// TODO: come up with a nice way to call this class in static methods
class WR_Logger<Class T>
{
	private string m_sLoggingContextName;
	private typename m_Type;
	
	void WR_Logger(T tInstance)
	{
		m_sLoggingContextName = tInstance.ClassName();
	}
	
	static void LogNormalStatic(string msg, Class c)
	{
		string className = c.ClassName();
		Print(FormatMessageStatic(msg, className));
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
		//return s_sLogPrefix + m_sLoggingContextName + ": " + msg;
		return FormatMessageStatic(msg, m_sLoggingContextName);
	}
	
	private static string FormatMessageStatic(string msg, string className)
	{
		return string.Format("[WASTELAND] %1: %2", className, msg);
	}
}

//class WR_StaticTest
//{
//	static void LogNormalStatic(string msg, Class c)
//	{
//		Print(FormatMessageStatic(msg, "classname"));
//	}
//	
//	private static string FormatMessageStatic(string msg, string className)
//	{
//		return "[WASTELAND]" + className + ": " + msg;
//	}
//	
//	static string ClassNameStatic()
//	{
//		Managed m = new Managed();
//		return m.ClassName();
//	}
//}