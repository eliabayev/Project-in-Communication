package bgu.spl171.net.srv;

public class RRQ extends Packet{
	private final short _opcode;
	String _fileName; //with '\0' at the end
	
	public RRQ(short opcode, String fileName){
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
