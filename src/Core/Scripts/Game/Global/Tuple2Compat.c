// AR 1.7 compatibility shim: vanilla scripts (e.g. SCR_ScenarioUICommon.c) still reference
// Tuple2, but it no longer resolves in this script module's compile scope under 1.7.
// Definition matches Bohemia's own Enfusion Script API (Tuple2<Class T1, Class T2> : Tuple).
class Tuple2<Class T1, Class T2> : Tuple
{
	T1 param1;
	T2 param2;

	void Tuple2(T1 p1, T2 p2)
	{
		param1 = p1;
		param2 = p2;
	}

	override bool Serialize(Serializer ctx)
	{
		return true;
	}

	override bool Deserializer(Serializer ctx)
	{
		return true;
	}
}
