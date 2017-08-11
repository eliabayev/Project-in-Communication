package bgu.spl171.net.srv;

public class DIRQ extends Packet{
	private final short _opcode;
	
	public DIRQ(short opcode){
		_opcode=opcode;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return "DIRQ";
	}
}
