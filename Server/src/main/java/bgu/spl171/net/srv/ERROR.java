package bgu.spl171.net.srv;

public class ERROR extends Packet{
	private final short _opcode;
	private short _errorCode;
	private String _errorMessage;
	
	public ERROR(short opcode, short errorCode, String errorMessage){
		_opcode=opcode;
		_errorCode=errorCode;
		_errorMessage=errorMessage;
	}
	
	public short getOpcode(){
		return _opcode;
	}
	
	public String getString(){
		return _errorMessage;
	}
	
	public short getErrorCode(){
		return _errorCode;
	}
}
