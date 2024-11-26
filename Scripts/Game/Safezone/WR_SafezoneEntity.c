class WR_SafezoneEntityClass: ScriptedGameTriggerEntityClass{};

class WR_SafezoneEntity: ScriptedGameTriggerEntity
{
	protected RplComponent m_RplComponent;
	
	override void EOnInit(IEntity owner)
	{
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!m_RplComponent)
			return;
	}
	
	override protected void OnActivate(IEntity ent)
	{
		doGodmode(ent, true);
	}
	
	override void OnDeactivate(IEntity ent)
	{	
		doGodmode(ent, false);
	}
	
	void doGodmode(IEntity ent, bool isSafe)
	{
		if (IsProxy()) return;
		
		SCR_ChimeraCharacter m_Character = SCR_ChimeraCharacter.Cast(ent);
		if(!m_Character) return;

		SCR_DamageManagerComponent m_DamageManager = SCR_DamageManagerComponent.Cast(m_Character.FindComponent(SCR_DamageManagerComponent));
		if(!m_DamageManager) return;
		m_DamageManager.EnableDamageHandling(true);
	}
	
	bool IsProxy()
	{
		if (!m_RplComponent)
			m_RplComponent = RplComponent.Cast(FindComponent(RplComponent));
		
		return (m_RplComponent && m_RplComponent.IsProxy());
	}	
};