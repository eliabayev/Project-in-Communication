package bgu.spl171.net.api.bidi;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.concurrent.ConcurrentLinkedQueue;

import bgu.spl171.net.srv.ACK;
import bgu.spl171.net.srv.BCAST;
import bgu.spl171.net.srv.DATA;
import bgu.spl171.net.srv.ERROR;
import bgu.spl171.net.srv.Packet;

public class TFTProtocol implements BidiMessagingProtocol<Packet> {
	private int _connectionId;
	private Connections<Packet> _connections;
	private ConcurrentLinkedQueue<byte[]> _data;
	private short _blockNumber;
	private int _dataSize;
	private String _fileName;
	private String _userName;
	private boolean _shouldTerminate;
	
    public void start(int connectionId, Connections<Packet> connections){
    	_connectionId=connectionId;
    	_connections=connections;
		_fileName="";
		_userName="";
		_dataSize=0;
		_blockNumber=1;
		_shouldTerminate=false;
		_data=new ConcurrentLinkedQueue<byte[]>();
		updateFileList();
    }
    
    public void updateFileList(){
    	((ConnectionsImpl<Packet>)_connections).getFilesNames().clear();;
    	String location = "Files";
    	File folder = new File(location);
    	File[] listOfFiles = folder.listFiles();
    	for(int i=0; i<listOfFiles.length; i++)
    		if(listOfFiles[i].isFile())
    			((ConnectionsImpl<Packet>)_connections).getFilesNames().add(listOfFiles[i].getName());
    }
    
    public void process(Packet message){
    	Packet replyMessage = null;
    	short opcode = message.getOpcode();
    	if(opcode<0 || opcode>10)
    		replyMessage = new ERROR((short)5, (short)4, "Illigal TFTP opration");
    	else{
    		if(opcode==7){ //LOGRQ
    			if(message.getString().charAt(0) == ' '){
    				replyMessage = new ERROR((short)5, (short)0, "Invalid input, empty username or username starts with space");
    			}
    			else if((_userName !="")||((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(message.getString()))
    				replyMessage=new ERROR((short)5, (short)7, "User already logged in");
    			else{
    				_userName=message.getString();
    				((ConnectionsImpl<Packet>)_connections).getUsersNames().add(message.getString());
    				replyMessage=new ACK((short)4, (short)0);
    			}		
    		}
    		
    		else if(opcode==2){ //WRQ
    			updateFileList();
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else if(message.getString().charAt(0) == ' ')
    				replyMessage = new ERROR((short)5, (short)0, "Invalid input, Cannot create a file without a name or file cannot start with space");
    			else if(((ConnectionsImpl<Packet>)_connections).getFilesNames().contains(message.getString()))
    				replyMessage = new ERROR((short)5, (short)5, "File already exists");
    			else{
    				_fileName=message.getString();
    				((ConnectionsImpl<Packet>)_connections).getFilesNames().add(message.getString()); // need to check!!
    				replyMessage=new ACK((short)4, (short)0);
    			}
    		}

    		else if(opcode==3) { //DATA
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else if(((DATA)message).getData().length>512)
    				replyMessage = new ERROR((short)5, (short)2, "Access violation");
    			else if(((DATA)message).getBlockNumber()!=_blockNumber)	
    				replyMessage = new ERROR((short)5, (short)2, "Access violation");
    			else if(((DATA)message).getData().length == 512){
    				_dataSize+=(int)((DATA)message).getPacketSize();
    				_data.add(((DATA)message).getData());
    				_blockNumber++;
    				replyMessage = new ACK((short)4, ((DATA)message).getBlockNumber());
    			}
    			else {
    				_dataSize+=(int)((DATA)message).getPacketSize();
    				_data.add(((DATA)message).getData());
    				ByteBuffer byteBuffer = ByteBuffer.allocate(_dataSize);
    				for(byte[] currentByteData : _data)
    					byteBuffer.put(currentByteData);
    				File file=new File("Files" + File.separator +_fileName);
    				try(FileOutputStream output = new FileOutputStream(file);
    						BufferedOutputStream out = new BufferedOutputStream(output)){
    					out.write(byteBuffer.array());
    					((ConnectionsImpl<Packet>)_connections).getFilesNames().add(_fileName);
    					_connections.send(_connectionId, new ACK((short)4, ((DATA)message).getBlockNumber()));
    					_data.clear();
    					_dataSize = 0;
    					_blockNumber = 1;
    					_connections.broadcast(new BCAST((short)9, (byte)1, message.getString()));
    				}
    				catch(FileNotFoundException e){
    					e.printStackTrace();
    				}
    				catch(IOException e){
    					e.printStackTrace();
    				}
    			}		
    		}
    		
    		else if(opcode==1){ //RRQ
    			updateFileList();
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else if(message.getString().charAt(0) == ' ')
    				replyMessage = new ERROR((short)5, (short)0, "Invalid input, Cannot read a file without a name or file cannot start with space");
    			else if(!((ConnectionsImpl<Packet>)_connections).getFilesNames().contains(message.getString()))
    				replyMessage = new ERROR((short)5, (short)1, "File not found");
    			else if(message.getString().charAt(0) == ' ')
    				replyMessage = new ERROR((short)5, (short)0, "Cannot create a file without a name");
    			else{
    				try(FileInputStream input = new FileInputStream("Files" + File.separator+message.getString());
    						BufferedInputStream in = new BufferedInputStream(input)){
    					byte[] fileBytes = new byte[in.available()];
    					in.read(fileBytes, 0, in.available()); 
    					AddByteArraysToData(fileBytes);
    					replyMessage = createDataPacket();
    				}
    				catch(FileNotFoundException e){
    					e.printStackTrace();
    				}
    				catch(IOException e){
    					e.printStackTrace();
    				}	
    			}	
    		}

    		else if(opcode==6){ //DIRQ
    			updateFileList();
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else{
    				String listOfNames="";
    				for(String currentName : ((ConnectionsImpl<Packet>)_connections).getFilesNames())
    					listOfNames=listOfNames+currentName+'\0';
    				byte[] listOfNamesBytes = listOfNames.getBytes();
    				AddByteArraysToData(listOfNamesBytes);
    				replyMessage = createDataPacket();
    				listOfNames="";
    				if(_data.isEmpty())
    					_blockNumber = 1;
    			}
    		}	
    		
    		else if(opcode==8){ //DELRQ
    			updateFileList();
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else if(message.getString().charAt(0) == ' ')
    				replyMessage = new ERROR((short)5, (short)0, "Invalid input, Cannot delete a file without a name or file cannot start with space");
    			else if(!((ConnectionsImpl<Packet>)_connections).getFilesNames().contains(message.getString()))
    				replyMessage = new ERROR((short)5, (short)1, "File not found");
    			else{
    				((ConnectionsImpl<Packet>)_connections).getFilesNames().remove(message.getString());
    				File delFile = new File("Files" + File.separator+message.getString());
    				delFile.delete();
    				_connections.send(_connectionId, new ACK((short)4, (short)0));
    				_connections.broadcast(new BCAST((short)9, (byte)0, message.getString()));
    			}
    		}

    		else if(opcode==10){ //DISC
    			if(!((ConnectionsImpl<Packet>)_connections).getUsersNames().contains(_userName))
    				replyMessage = new ERROR((short)5, (short)6, "User not logged in");
    			else{
    				((ConnectionsImpl<Packet>)_connections).getUsersNames().remove(_userName);
    				_shouldTerminate = true;
    				replyMessage=new ACK((short)4, (short)0);
    				_userName="";
    			}
    		}

    		else if(opcode==5){ //ERROR
    			_shouldTerminate = true;
    		}

    		else if(opcode==4) //ACK
    			if(((ACK)message).getBlock()!=0){
    				if(!_data.isEmpty())
    					replyMessage = createDataPacket();
    				else
    					_blockNumber=1;
    			}	
    	}
    	if(replyMessage!=null){
    		_connections.send(_connectionId, replyMessage);
    	}
    	
    }
	
    private void AddByteArraysToData(byte[] array){
		int available = array.length;
		int pos = 0;
		for(int i=0;i<(array.length/512)+1;i++){
			if(available>=512){
				_data.add(Arrays.copyOfRange(array, pos, pos+512));
				pos=pos+512;
				available=available-512;
			}
			else
				_data.add(Arrays.copyOfRange(array, pos, array.length));
		}
    }
    
    private Packet createDataPacket(){
        byte[] firstData = _data.poll();
        Packet dataBlock = new DATA((short)3, (short)firstData.length, _blockNumber, firstData);
        _blockNumber++; 
        return dataBlock;
    }
    
	/**
     * @return true if the connection should be terminated
     */
    public boolean shouldTerminate(){
    	if(_shouldTerminate){
    		_connections.disconnect(_connectionId);
    		((ConnectionsImpl<Packet>)_connections).getUsersNames().remove(_userName);
    	}
    	return _shouldTerminate;
    }
}
