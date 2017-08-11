package bgu.spl171.net.srv;

public class DATA extends Packet{
	private final short _opcode;
	private short _packetSize;
	private short _blockNumber;
	private byte[] _data;
	
	public DATA(short opcode ,short packetSize, short blockNumber, byte[] data){
		_opcode=opcode;
		_packetSize=packetSize;
		_blockNumber=blockNumber;	
		_data=data;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return "DATA";
	}
	
	public short getBlockNumber(){
		return _blockNumber;
	}
	
	public byte[] getData(){
		return _data;
	}
	
	public short getPacketSize(){
		return _packetSize;
	}
}
