package bgu.spl171.net.srv;

public class BCAST extends Packet{
	private final short _opcode;
	private byte _deletedOrAdded;
	private String _fileName;
	
	public BCAST(short opcode, byte deletedOrAdded, String fileName){
		_opcode=opcode;
		_deletedOrAdded=deletedOrAdded;
		_fileName=fileName;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return _fileName;
	}
	
	public byte getDeletedOrAdded(){
		return _deletedOrAdded;
	}
}
