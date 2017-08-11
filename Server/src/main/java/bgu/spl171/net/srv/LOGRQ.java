package bgu.spl171.net.srv;

public class LOGRQ extends Packet{
	private final short _opcode;
	private String _userName;
	
	public LOGRQ(short opcode, String userName){
		_opcode=opcode;
		_userName=userName;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return _userName;
	}
}
