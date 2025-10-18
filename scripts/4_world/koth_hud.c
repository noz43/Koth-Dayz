class KOTH_HUD
{
    protected Widget m_Root;
    protected Widget m_HandlePanel;
    protected Widget m_InfoPanel;
    protected TextWidget m_ZoneText;
    protected TextWidget m_PlayerCount;
    protected TextWidget m_ProgressText;
    protected ProgressBarWidget m_ProgressBar;
    protected ImageWidget m_Icon;
    protected ref KOTH_HUDData m_serverData;
    protected bool m_IsVisible = false;

    void KOTH_HUD()
    {
        CreateHUD();
    }

    void ~KOTH_HUD()
    {
        if (m_Root)
        {
            m_Root.Unlink();
        }
    }

    protected void CreateHUD()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("KOTH-SAVIOR/gui/layouts/kothprogressbar.layout");
        
        if (!m_Root)
        {
            m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/kothprogressbar.layout");
        }
        
        if (!m_Root)
            return;
        
        m_HandlePanel = m_Root.FindAnyWidget("MainPanel");
        m_ProgressBar = ProgressBarWidget.Cast(m_Root.FindAnyWidget("ProgressBar"));
        m_ProgressText = TextWidget.Cast(m_Root.FindAnyWidget("ProgressText"));
        m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("Icon"));

        Widget infoPanelWidget = m_Root.FindAnyWidget("InfoPanel");
        if (infoPanelWidget)
        {
            infoPanelWidget.Show(false);
        }

        if (m_Icon)
        {
            array<string> iconCandidates = {
                "KOTH-SAVIOR/gui/images/logosavior.edds", 
                "KOTH-SAVIOR/gui/images/logosavior.paa",  
                "logosavior.edds",                         
                "logosavior.paa"
            };
            
            bool iconLoaded = false;
            foreach (string p : iconCandidates)
            {
                if (FileExist(p))
                {
                    if (m_Icon.LoadImageFile(0, p))
                    {
                        m_Icon.SetImage(0);
                        m_Icon.Show(true);
                        iconLoaded = true;
                        break;
                    }
                }
            }
            
            if (!iconLoaded)
            {
                string fallbackPath = "gui/textures/compass.paa";
                if (m_Icon.LoadImageFile(0, fallbackPath))
                {
                    m_Icon.SetImage(0);
                    m_Icon.Show(true);
                }
            }
        }

        if (m_ProgressBar)
        {
            m_ProgressBar.SetCurrent(0);
        }
            
        if (m_ProgressText)
        {
            m_ProgressText.SetText("0%");
            m_ProgressText.Show(true);
        }

        HandleShow(false);
    }

    void HandleShow(bool show)
    {
        if (!m_Root)
            return;
        
        if (m_HandlePanel)
            m_HandlePanel.Show(show);
            
        m_IsVisible = show;
    }

    void UpdateProgress(float progress)
    {
        if (!m_IsVisible || !m_ProgressBar)
            return;
            
        if (progress < 0) progress = 0;
        if (progress > 100) progress = 100;
        
        m_ProgressBar.SetCurrent(progress);
        
        if (m_ProgressText)
        {
            m_ProgressText.SetText(string.Format("%1%%", Math.Round(progress).ToString()));
        }
    }

    void UpdatePlayerCount(string playerCount)
    {
    }

    void SetVisible(bool visible)
    {
        HandleShow(visible);
    }

    bool IsVisible()
    {
        return m_IsVisible;
    }
    
    void UpdateStatus(string status)
    {
    }
    
    void OnScreenSizeChanged()
    {
    }
}

class KOTH_HUDData
{
    protected string m_PlayerCount;
    protected float m_Progress;
    protected bool m_InZone;

    void SetData(string playerCount, float progress, bool inZone)
    {
        m_PlayerCount = playerCount;
        m_Progress = progress;
        m_InZone = inZone;
    }

    string GetPlayerCount()
    {
        return m_PlayerCount;
    }

    float GetProgress()
    {
        return m_Progress;
    }

    bool IsInZone()
    {
        return m_InZone;
    }
}