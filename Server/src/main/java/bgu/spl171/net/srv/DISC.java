package bgu.spl171.net.srv;

public class DISC extends Packet{
	private final short _opcode;
	
	public DISC(short opcode){
		_opcode=opcode;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return "DISC";
	}
}
