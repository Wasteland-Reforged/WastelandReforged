modded class SCR_SupportStationManagerComponent : ScriptComponent
{
    protected static SCR_SupportStationManagerComponent s_Instance;

    override void OnPostInit(IEntity owner)
    {
        s_Instance = this;
        super.OnPostInit(owner);
    }
}