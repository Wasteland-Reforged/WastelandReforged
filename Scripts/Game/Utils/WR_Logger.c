// TODO: come up with a nice way to call this class in static methods
class WR_Logger<Class T>
{
	private const string m_sLogPrefix = "[WASTELAND] ";
	private string m_sLoggingContextName;
	
	void WR_Logger(T tInstance)
	{
		m_sLoggingContextName = tInstance.ClassName() + ": ";
	}

	void LogSpam(string msg)
	{
		Print(FormatMessage(msg), LogLevel.SPAM);
	}
	
	void LogVerbose(string msg)
	{
		Print(FormatMessage(msg), LogLevel.VERBOSE);
	}
	
	void LogDebug(string msg)
	{
		Print(FormatMessage(msg), LogLevel.DEBUG);
	}
	
	void LogNormal(string msg)
	{
		Print(FormatMessage(msg), LogLevel.NORMAL);
	}
	
	void LogWarning(string msg)
	{
		Print(FormatMessage(msg), LogLevel.WARNING);
	}
	
	void LogError(string msg)
	{
		Print(FormatMessage(msg), LogLevel.ERROR);
	}
	
	void LogFatal(string msg)
	{
		Print(FormatMessage(msg), LogLevel.VERBOSE);
	}
	
	private string FormatMessage(string msg)
	{
		return m_sLogPrefix + m_sLoggingContextName + msg;
	}
}