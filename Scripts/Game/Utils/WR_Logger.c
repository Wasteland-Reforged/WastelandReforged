class WR_Logger
{
	private const string m_sLogPrefix = "[WASTELAND] ";
	
	static void LogSpam(string msg)
	{
		Print(FormatMessage(msg), LogLevel.SPAM);
	}
	
	static void LogVerbose(string msg)
	{
		Print(FormatMessage(msg), LogLevel.VERBOSE);
	}
	
	static void LogDebug(string msg)
	{
		Print(FormatMessage(msg), LogLevel.DEBUG);
	}
	
	static void LogNormal(string msg)
	{
		Print(FormatMessage(msg), LogLevel.NORMAL);
	}
	
	static void LogWarning(string msg)
	{
		Print(FormatMessage(msg), LogLevel.WARNING);
	}
	
	static void LogError(string msg)
	{
		Print(FormatMessage(msg), LogLevel.ERROR);
	}
	
	static void LogFatal(string msg)
	{
		Print(FormatMessage(msg), LogLevel.VERBOSE);
	}
	
	private static string FormatMessage(string msg)
	{
		return m_sLogPrefix + msg;
	}
}