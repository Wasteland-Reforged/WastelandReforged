[ComponentEditorProps(category: "Tutorial/Component", description: "Warn then teleport humans that are too close to the entity")]
class WR_TeleportFieldComponentClass : ScriptComponentClass
{
}

class WR_TeleportFieldComponent : ScriptComponent
{
	/*
		Teleportation
	*/

	[Attribute(defvalue: "10", desc: "Distance at which the field draws a line to its target to warn it about teleportation", category: "Teleportation")]
	protected float m_fWarningRadius;

	[Attribute(defvalue: "2", desc: "Distance at which the field triggers the teleportation", params: "0.25 10 0.25", category: "Teleportation")]
	protected float m_fTriggerRadius;

	[Attribute(defvalue: "10", desc: "Distance at which the teleportation places the unit from the teleporter", category: "Teleportation")]
	protected float m_fTeleportDistance;

	/*
		Line Drawing
	*/

	[Attribute(defvalue: "1 0.75 0 1", desc: "The line's colour", category: "Line Drawing")]
	protected ref Color m_LineColour;

	[Attribute(defvalue: "1", desc: "Whether or not the line must fade in/out with transparency based on distance", category: "Line Drawing")]
	protected bool m_bLineFadeInOut;

	[Attribute(defvalue: "0 1 0", desc: "The line offset from entities's origins", category: "Line Drawing")]
	protected vector m_vOffset;

	/*
		Performance
	*/

	[Attribute(defvalue: "0.25", desc: "Duration between proximity checks", category: "Performance")]
	protected float m_fCheckPeriod;

	protected float m_fCheckDelay;
	protected int m_iTempLineColour;
	protected ref array<ref Shape> m_aShapes;
	protected ref array<IEntity> m_aNearbyCharacters;
	
	private static MenuBase menu;

	//------------------------------------------------------------------------------------------------
	//! Draw a debug line between two entities
	//! \param[in] from
	//! \param[in] to
	//! \param[in] offset
	protected Shape DrawLine(notnull IEntity from, notnull IEntity to, vector offset)
	{
		vector points[2] = { from.GetOrigin() + offset, to.GetOrigin() + offset };
		float distance = vector.Distance(points[0], points[1]);
		if (m_bLineFadeInOut)
		{
			int alpha255 = 255 * (1 - ((distance - m_fTriggerRadius) / (m_fWarningRadius - m_fTriggerRadius)));
			m_iTempLineColour = m_iTempLineColour & 0x00FFFFFF | (alpha255 << 24);
			return Shape.CreateLines(m_iTempLineColour, ShapeFlags.TRANSP, points, 2);
		}
		else
		{
			return Shape.CreateLines(m_iTempLineColour, 0, points, 2);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);

		vector ownerPos = owner.GetOrigin();

		m_fCheckDelay -= timeSlice;
		if (m_fCheckDelay <= 0)
		{
			m_fCheckDelay = m_fCheckPeriod;

			m_aNearbyCharacters.Clear();
			owner.GetWorld().QueryEntitiesBySphere(ownerPos, m_fWarningRadius, QueryEntitiesCallbackMethod, null, EQueryEntitiesFlags.DYNAMIC | EQueryEntitiesFlags.WITH_OBJECT);
		}

		m_aShapes.Clear();
		m_aShapes.Reserve(m_aNearbyCharacters.Count());
		foreach (IEntity character : m_aNearbyCharacters)
		{
			vector characterPos = character.GetOrigin();
			if (vector.Distance(characterPos, ownerPos) > m_fTriggerRadius)			// in the warning zone
			{
				m_aShapes.Insert(DrawLine(owner, character, m_vOffset));	// draw line
			}
			else										// in the trigger zone
			{
				vector dir = vector.Direction(ownerPos, characterPos).Normalized();
				character.SetOrigin(ownerPos + dir * m_fTeleportDistance);	// teleport
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	// QueryEntitiesCallback type
	protected bool QueryEntitiesCallbackMethod(IEntity e)
	{
		if (!e || !ChimeraCharacter.Cast(e)) // only humans
			return false;

		m_aNearbyCharacters.Insert(e);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnPostInit(IEntity owner)
	{
		m_aShapes = {};
		m_aNearbyCharacters = {};
		m_iTempLineColour = m_LineColour.PackToInt();
		SetEventMask(owner, EntityEvent.FRAME);
	}
}