class KOTH_Log
{
	protected static bool m_Initialized;
	
	protected static FileHandle m_LogFileHandle;

	protected static int m_Day, m_Hour, m_LastDay, m_Minute, m_Month, m_Second, m_Year = 0;

	protected static string m_RootDirectory = "$profile:KOTH-SAVIOR";
	protected static string m_LogDirectory = m_RootDirectory + "/Logs";
	protected static string m_Date;

	protected static string GetDate()
	{
		GetYearMonthDay(m_Year, m_Month, m_Day);
		
		if (m_LastDay != m_Day)
			m_Date = string.Format("%1-%2-%3", m_Year.ToStringLen(2), m_Month.ToStringLen(2), m_Day.ToStringLen(2));
		
		m_LastDay = m_Day;

		return m_Date;
	}

    protected static string GetLogFile()
    {
        return string.Format("%1/KOTH-%2.log", m_LogDirectory, GetDate());
    }

    protected static string GetTime()
    {
        GetHourMinuteSecond(m_Hour, m_Minute, m_Second);
        
		return string.Format("%1:%2:%3", m_Hour.ToStringLen(2), m_Minute.ToStringLen(2), m_Second.ToStringLen(2));
    }

    protected static void WriteLine(string input)
    {
		if (!FileExist(m_RootDirectory))
			MakeDirectory(m_RootDirectory);

		if (!FileExist(m_LogDirectory))
			MakeDirectory(m_LogDirectory);

		m_LogFileHandle = OpenFile(GetLogFile(), FileMode.APPEND);
        if (m_LogFileHandle != 0)
		{
			if (!m_Initialized)
			{
				FPrintln(m_LogFileHandle, "[KOTH] -----------------------------------------------");
				FPrintln(m_LogFileHandle, string.Format("[KOTH] Session start at %1", GetTime()));
				FPrintln(m_LogFileHandle, "[KOTH] -----------------------------------------------");
				m_Initialized = true;
			}
			
			FPrintln(m_LogFileHandle, string.Format("[KOTH] [%1] %2", GetTime(), input));
			CloseFile(m_LogFileHandle);
		}
    }

	static void LogBasic(string message)
	{
		if (KOTH_Settings.GetLoggingLevel() >= eKOTH.LOG_BASIC)
			WriteLine(string.Format("%1", message));
	}

	static void LogCritical(string message)
	{
		if (KOTH_Settings.GetLoggingLevel() >= eKOTH.LOG_CRITICAL)
			WriteLine(string.Format("[CRITICAL] %1", message));
	}

	static void LogRaw(string message)
	{
		WriteLine(string.Format("%1", message));
	}

	static void LogVerbose(string message)
	{
		if (KOTH_Settings.GetLoggingLevel() >= eKOTH.LOG_VERBOSE)
			WriteLine(string.Format("%1", message));
	}
}