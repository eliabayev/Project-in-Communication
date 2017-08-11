package bgu.spl171.net.srv;

public class ACK extends Packet{
	private final short _opcode;
	private short _block;
	
	public ACK(short opcode, short block){
		_block=block;
		_opcode=opcode;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return "ACK";
	}
	
	public short getBlock(){
		return _block;
	}
}
