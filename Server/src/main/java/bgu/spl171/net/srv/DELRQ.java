package bgu.spl171.net.srv;

public class DELRQ extends Packet{
	private final short _opcode;
	private String _fileName;
	
	public DELRQ(short opcode, String fileName){
		_opcode=opcode;
		_fileName=fileName;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return _fileName;
	}
}
