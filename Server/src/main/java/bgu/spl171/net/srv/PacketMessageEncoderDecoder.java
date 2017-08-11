package bgu.spl171.net.srv;

import java.nio.charset.StandardCharsets;

import bgu.spl171.net.api.MessageEncoderDecoder;

public class PacketMessageEncoderDecoder implements MessageEncoderDecoder<Packet> {
    private byte[] _bytes = new byte[1 << 10]; //starts with 1k
    private int _len = 0;
    private short _opcode;
	
    public Packet decodeNextByte(byte nextByte){
        if (_len<=2) {
        	_bytes[_len++]=nextByte; 
        	if(_len==2){
        		_opcode= bytesToShort(0);
        		if((_opcode==6)||(_opcode==10))
        			return createPacket();
        	}
        } 
        else{
        	if(_opcode==5){ //ERROR
        		if(_len<=4)
        			_bytes[_len++]=nextByte; 
        		else
        			return checkForZeroByte(nextByte);
        	}
        	else if(_opcode==9){ //BCAST
        		if(_len<=3)
        			_bytes[_len++]=nextByte; 
        		else
        			return checkForZeroByte(nextByte);
        	}
        	else if((_opcode==1)||(_opcode==2)||(_opcode==7)||(_opcode==8)){
        		return checkForZeroByte(nextByte);
    		}
    		else if(_opcode==4){ //ACK
				_bytes[_len++]=nextByte; 
    			if(_len==4)
    				return createPacket();
    		}
    		else if(_opcode==3){ //DATA
    			_bytes[_len++]=nextByte; 
    			if(_len>=6){
    				if(bytesToShort(2)+6==_len)
    					return createPacket();			
    			}	
    		}	
        }
        return null; //not a line yet
	}
    
    private Packet checkForZeroByte(byte nextByte){
    	if(nextByte!='\0')
    		_bytes[_len++]=nextByte;
    	else
    		return createPacket();
    	return null;
    }
    
    private Packet createPacket(){
    	Packet result = null;
		if(_opcode==1) result = active1278();
		else if(_opcode==2) result = active1278();
		else if(_opcode==3) result = activeDATA();
		else if(_opcode==4) result = activeACK();
		else if(_opcode==5) result = activeERROR();
		else if(_opcode==6) {result = new DIRQ((short)6); clearBytes(); _len=0;}
		else if(_opcode==7) result = active1278();
		else if(_opcode==8) result = active1278();
		else if(_opcode==9) result = activeBCAST();
    	else if(_opcode==10) {result = new DISC((short)10); clearBytes(); _len=0;}
		return result;
    }
    
    private Packet active1278(){
    	Packet packet = null;
    	String name=convertBytesToString(2);
    	switch(_opcode){
    	case 1 : packet = new RRQ((short)1, name); clearBytes(); _len=0; break;
    	case 2 : packet = new WRQ((short)2, name); clearBytes(); _len=0; break;
    	case 7 : packet = new LOGRQ((short)7, name); clearBytes(); _len=0; break;
    	case 8 : packet = new DELRQ((short)8, name); clearBytes(); _len=0; break;
    	}
    	return packet;
    }
    
    private Packet activeDATA(){
    	short packetSize=bytesToShort(2);
    	short blockNumber=bytesToShort(4);
    	byte[] dataBytes=new byte[_len-6];
    	for(int i=6;i<_len;i++)
    		dataBytes[i-6]=_bytes[i];
    	clearBytes();
    	_len=0;
    	return new DATA((short)3, packetSize, blockNumber, dataBytes);
    	
    }
    
    private Packet activeBCAST(){
    	byte deletedOrAdded = _bytes[2];
    	String bcastName=convertBytesToString(3);
    	clearBytes();
    	_len=0;
    	return new BCAST((short)9, deletedOrAdded, bcastName);
    } 
    
    private Packet activeACK(){
    	short blockNumber=bytesToShort(2);
    	clearBytes();
    	_len=0;
    	return new ACK((short)4, blockNumber);
    }
    
    private Packet activeERROR(){
    	short errorCode = bytesToShort(2);
    	String errorName=convertBytesToString(4);
    	clearBytes();
    	_len=0;
    	return new ERROR((short)5, errorCode, errorName);
    }
    
    public byte[] encode(Packet message){
    	short opcode=message.getOpcode();
		byte[] opcodeBytes=shortToBytes(opcode);
    	if((opcode==1)||(opcode==2)||(opcode==7)||(opcode==8)){ 
    		byte[] nameBytes = message.getString().getBytes();
    		byte[] messageBytes = new byte[nameBytes.length + 3];
    		messageBytes[0]=opcodeBytes[0];
    		messageBytes[1]=opcodeBytes[1];
    		messageBytes[messageBytes.length-1]=0;
    		for(int i=2;i<messageBytes.length-1;i++){
    			messageBytes[i]=nameBytes[i-2];
    		}	
    		return messageBytes;
    	}
    	if(opcode==4){//ACK
    		byte[] blockBytes=shortToBytes(((ACK)message).getBlock());
    		byte[] messageBytes = new byte[4];
    		messageBytes[0]=opcodeBytes[0];
    		messageBytes[1]=opcodeBytes[1];
    		messageBytes[2]=blockBytes[0];
    		messageBytes[3]=blockBytes[1];
    		return messageBytes;
    	}
    	if(opcode==5){ //ERROR
    		byte[] errorCodeBytes=shortToBytes(((ERROR)message).getErrorCode());
    		byte[] nameBytes = message.getString().getBytes();
    		byte[] messageBytes = new byte[nameBytes.length + 5];
    		messageBytes[0]=opcodeBytes[0];
    		messageBytes[1]=opcodeBytes[1];
    		messageBytes[2]=errorCodeBytes[0];
    		messageBytes[3]=errorCodeBytes[1];
    		messageBytes[messageBytes.length-1]=0;
    		for(int i=4;i<messageBytes.length-1;i++){
    			messageBytes[i]=nameBytes[i-4];
    		}	
    		return messageBytes;
    	}
    	if(opcode==9){ //BCAST
    		byte[] nameBytes = message.getString().getBytes();
    		byte[] messageBytes = new byte[nameBytes.length + 4];
    		messageBytes[0]=opcodeBytes[0];
    		messageBytes[1]=opcodeBytes[1];
    		messageBytes[2]=((BCAST)message).getDeletedOrAdded();
    		messageBytes[messageBytes.length-1]=0;
    		for(int i=3;i<messageBytes.length-1;i++){
    			messageBytes[i]=nameBytes[i-3];
    		}	
    		return messageBytes;
    	}
    	if(opcode==3){ //DATA
    		byte[] dataBytes = ((DATA)message).getData();
    		byte[] blockNumberBytes = shortToBytes(((DATA)message).getBlockNumber());
    		byte[] packetSizeBytes = shortToBytes(((DATA)message).getPacketSize());
    		byte[] messageBytes = new byte[dataBytes.length + 6];
    		messageBytes[0]=opcodeBytes[0];
    		messageBytes[1]=opcodeBytes[1];
    		messageBytes[2]=packetSizeBytes[0];
    		messageBytes[3]=packetSizeBytes[1];
    		messageBytes[4]=blockNumberBytes[0];
    		messageBytes[5]=blockNumberBytes[1];
    		for(int i=6;i<messageBytes.length;i++){
    			messageBytes[i]=dataBytes[i-6];
    		}	
    		return messageBytes;
    	}
    	return opcodeBytes;
    }
    
    private void clearBytes(){
    	for(int i=0;i<_len;i++)
    		_bytes[i]=0;
    }
    
    private String convertBytesToString(int startPoint) {
        String result = new String(_bytes, startPoint, _len-startPoint, StandardCharsets.UTF_8);
        return result;
    }
    
    private short bytesToShort(int startPoint) {
        short result = (short)((_bytes[startPoint] & 0xff) << 8);
        result += (short)(_bytes[startPoint+1] & 0xff);
        return result;
    }
    
    private byte[] shortToBytes(short num) {
        byte[] bytesArr = new byte[2];
        bytesArr[0] = (byte)((num >> 8) & 0xFF);
        bytesArr[1] = (byte)(num & 0xFF);
        return bytesArr;
    }
}
