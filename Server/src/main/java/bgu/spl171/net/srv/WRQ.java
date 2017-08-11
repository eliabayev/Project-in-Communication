package bgu.spl171.net.srv;

public class WRQ extends Packet{
	private final short _opcode;
	String _fileName; //with '\0' at the end
	
	public WRQ(short opcode, String fileName){
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
