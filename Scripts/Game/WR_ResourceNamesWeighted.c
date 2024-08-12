class WR_ResourceNamesWeighted
{
    private static ref WR_WeightedItemArray<ResourceName> Rifles;
    private static ref WR_WeightedItemArray<ResourceName> MachineGuns;
    private static ref WR_WeightedItemArray<ResourceName> Snipers;
    private static ref WR_WeightedItemArray<ResourceName> Handguns;
    private static ref WR_WeightedItemArray<ResourceName> Launchers;
    private static ref WR_WeightedItemArray<ResourceName> Ordnance;
    private static ref WR_WeightedItemArray<ResourceName> Attachments;
    private static ref WR_WeightedItemArray<ResourceName> UtilityItems;
    private static ref WR_WeightedItemArray<ResourceName> MedicalItems;
    private static ref WR_WeightedItemArray<ResourceName> Vests;
    private static ref WR_WeightedItemArray<ResourceName> Helmets;
    private static ref WR_WeightedItemArray<ResourceName> Hats;
    private static ref WR_WeightedItemArray<ResourceName> Backpacks;
    private static ref WR_WeightedItemArray<ResourceName> NonReloadableWeapons;
	
	static WR_WeightedItemArray<ResourceName> GetRifles()
	{
		if (Rifles) return Rifles;
		Rifles = new WR_WeightedItemArray<ResourceName>();

		Rifles.AddItem(11.0, "{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et");
		Rifles.AddItem(7.0, "{F97A4AC994231900}Prefabs/Weapons/Rifles/M16/Rifle_M16A2_carbine.et");
		Rifles.AddItem(2.5, "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et");
		Rifles.AddItem(5.5, "{96DFD2E7E63B3386}Prefabs/Weapons/Rifles/AK74/Rifle_AK74N.et");
		Rifles.AddItem(2.5, "{BFEA719491610A45}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74U.et");
		Rifles.AddItem(5.5, "{FA0E25CE35EE945F}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74UN.et");
		Rifles.AddItem(6.0, "{9C948630078D154D}Prefabs/Weapons/Rifles/VZ58/Rifle_VZ58P.et");
		Rifles.AddItem(6.0, "{443CEFF17E040B11}Prefabs/Weapons/Rifles/VZ58/Rifle_VZ58V.et");

		return Rifles;	
	}

	static WR_WeightedItemArray<ResourceName> GetMachineGuns()
	{
		if (MachineGuns) return MachineGuns;
		MachineGuns = new WR_WeightedItemArray<ResourceName>();

		MachineGuns.AddItem(4.0, "{D2B48DEBEF38D7D7}Prefabs/Weapons/MachineGuns/M249/MG_M249.et");
		MachineGuns.AddItem(5.0, "{D182DCDD72BF7E34}Prefabs/Weapons/MachineGuns/M60/MG_M60.et");
		MachineGuns.AddItem(6.0, "{A89BC9D55FFB4CD8}Prefabs/Weapons/MachineGuns/PKM/MG_PKM.et");
		MachineGuns.AddItem(6.0, "{5F365605E36597FB}Prefabs/Weapons/MachineGuns/RPK74/MG_RPK74N.et");
		MachineGuns.AddItem(6.0, "{026CE108BFB3EC03}Prefabs/Weapons/MachineGuns/UK59/MG_UK59.et");

		return MachineGuns;	
	}

	static WR_WeightedItemArray<ResourceName> GetSnipers()
	{
		if (Snipers) return Snipers;
		Snipers = new WR_WeightedItemArray<ResourceName>();

		Snipers.AddItem(6.0, "{3EB02CDAD5F23C82}Prefabs/Weapons/Rifles/SVD/Rifle_SVD.et");
		Snipers.AddItem(4.0, "{B31929F65F0D0279}Prefabs/Weapons/Rifles/M14/Rifle_M21.et");

		return Snipers;	
	}

	static WR_WeightedItemArray<ResourceName> GetHandguns()
	{
		if (Handguns) return Handguns;
		Handguns = new WR_WeightedItemArray<ResourceName>();

		Handguns.AddItem(1.0, "{1353C6EAD1DCFE43}Prefabs/Weapons/Handguns/M9/Handgun_M9.et");
		Handguns.AddItem(1.0, "{C0F7DD85A86B2900}Prefabs/Weapons/Handguns/PM/Handgun_PM.et");

		return Handguns;	
	}

	static WR_WeightedItemArray<ResourceName> GetLaunchers()
	{
		if (Launchers) return Launchers;
		Launchers = new WR_WeightedItemArray<ResourceName>();

		Launchers.AddItem(5.0, "{9C5C20FB0E01E64F}Prefabs/Weapons/Launchers/M72/Launcher_M72A3.et");
		Launchers.AddItem(3.0, "{7A82FE978603F137}Prefabs/Weapons/Launchers/RPG7/Launcher_RPG7.et");

		return Launchers;	
	}

	static WR_WeightedItemArray<ResourceName> GetOrdnance()
	{
		if (Ordnance) return Ordnance;
		Ordnance = new WR_WeightedItemArray<ResourceName>();

		// Frag grenades
		Ordnance.AddItem(8.0, "{E8F00BF730225B00}Prefabs/Weapons/Grenades/Grenade_M67.et");
		Ordnance.AddItem(8.0, "{645C73791ECA1698}Prefabs/Weapons/Grenades/Grenade_RGD5.et");

		// Smoke grenades
		Ordnance.AddItem(3.0, "{9DB69176CEF0EE97}Prefabs/Weapons/Grenades/Smoke_ANM8HC.et");
		Ordnance.AddItem(1.5, "{3343A055A83CB30D}Prefabs/Weapons/Grenades/M18/Smoke_M18_Red.et");
		Ordnance.AddItem(1.5, "{D41D22DD1B8E921E}Prefabs/Weapons/Grenades/M18/Smoke_M18_Green.et");
		Ordnance.AddItem(1.5, "{14C1A0F061D9DDEE}Prefabs/Weapons/Grenades/M18/Smoke_M18_Violet.et");
		Ordnance.AddItem(1.5, "{9BBDEE253A16CC66}Prefabs/Weapons/Grenades/M18/Smoke_M18_Yellow.et");
		Ordnance.AddItem(3.0, "{77EAE5E07DC4678A}Prefabs/Weapons/Grenades/Smoke_RDG2.et");

		// UGL frag ammo
		Ordnance.AddItem(3.0, "{5375FA7CB1F68573}Prefabs/Weapons/Ammo/Ammo_Grenade_HE_M406.et");
		Ordnance.AddItem(3.0, "{1663496AE5B9F10B}Prefabs/Weapons/Ammo/Ammo_Grenade_HEDP_M433.et");
		Ordnance.AddItem(6.0, "{262F0D09C4130826}Prefabs/Weapons/Ammo/Ammo_Grenade_HE_VOG25.et");

		// UGL flare ammo
		Ordnance.AddItem(10.0, "{98DB57ECEDC81CC2}Prefabs/Weapons/Ammo/Ammo_Flare_40mm_M583A1_White.et");
		Ordnance.AddItem(10.0, "{0AF10C206CF1A283}Prefabs/Weapons/Ammo/Ammo_Flare_40mm_M661_Green.et");
		Ordnance.AddItem(10.0, "{2A63C909016C4C41}Prefabs/Weapons/Ammo/Ammo_Flare_40mm_M662_Red.et");
		Ordnance.AddItem(30.0, "{906F07BD0366E08F}Prefabs/Weapons/Ammo/Ammo_Flare_40mm_VG40OP_White.et");
		
		// Explosive charges
		Ordnance.AddItem(6.0, "{33CBDE73AB48172A}Prefabs/Weapons/Explosives/DemoBlock_M112/DemoBlock_M112.et");
		Ordnance.AddItem(6.0, "{97064F8597F2D7BF}Prefabs/Weapons/Explosives/DemoBlock_TSh400g/DemoBlock_TSh400g.et");
		
		// Anti-tank mines
		Ordnance.AddItem(5.0, "{49FFE8F373F55960}Prefabs/Weapons/Explosives/Mine_M15AT/Mine_M15AT.et");
		Ordnance.AddItem(5.0, "{D6EF54367CECE1D9}Prefabs/Weapons/Explosives/Mine_TM62M/Mine_TM62M.et");

		return Ordnance;	
	}

	static WR_WeightedItemArray<ResourceName> GetAttachments()
	{
		if (Attachments) return Attachments;
		Attachments = new WR_WeightedItemArray<ResourceName>();

		// Sights
		Attachments.AddItem(4.0, "{08286DDBB1F33FF1}Prefabs/Weapons/Attachments/Optics/Optic_AP2k/Collim_AP2k.et");
		Attachments.AddItem(4.0, "{BD496EE1B40DC510}Prefabs/Weapons/Attachments/Optics/Optic_4x20/Optic_4x20.et");
		Attachments.AddItem(4.0, "{D2018EDB1BBF4C88}Prefabs/Weapons/Attachments/Optics/Optic_ARTII/Optic_ARTII.et");
		Attachments.AddItem(4.0, "{ACDF49FACD0701A8}Prefabs/Weapons/Attachments/Optics/Optic_1P29/Optic_1P29.et");
		Attachments.AddItem(4.0, "{E5E9DBBF3BFB88C6}Prefabs/Weapons/Attachments/Optics/Optic_PGO7/Optic_PGO7V3.et");
		Attachments.AddItem(4.0, "{C850A33226B8F9C1}Prefabs/Weapons/Attachments/Optics/Optic_PSO1/Optic_PSO1.et");

		// Muzzle attachments
		Attachments.AddItem(6.0, "{4A815EB8B824974A}Prefabs/Weapons/Attachments/Muzzle/FlashHider_AK74/FlashHider_AK74.et");
		Attachments.AddItem(6.0, "{06D4C36A6D585275}Prefabs/Weapons/Attachments/Muzzle/FlashHider_AKS74u/FlashHider_AKS74u.et");
		Attachments.AddItem(6.0, "{6288A1F1A5E3AC37}Prefabs/Weapons/Attachments/Muzzle/FlashHider_M16A2/FlashHider_M16.et");

		// Underbarrel attachments
		Attachments.AddItem(3.0, "{1ABABE3551512B0A}Prefabs/Weapons/Attachments/Underbarrel/UGL_GP25.et");
		Attachments.AddItem(3.0, "{AB268D088F2D6291}Prefabs/Weapons/Attachments/Underbarrel/UGL_M203_short.et");

		// Bayonets
		Attachments.AddItem(3.0, "{558117556F3880A8}Prefabs/Weapons/Attachments/Bayonets/Bayonet_M9.et");
		Attachments.AddItem(3.0, "{98C79F5FAE12F9B6}Prefabs/Weapons/Attachments/Bayonets/Bayonet_6Kh4.et");
		Attachments.AddItem(1.0, "{3F1B1BAD1F6DDC28}Prefabs/Weapons/Attachments/Bayonets/Bayonet_VZ58/Bayonet_VZ58_v1.et");
		Attachments.AddItem(1.0, "{F4B9ADFE20D0D6B8}Prefabs/Weapons/Attachments/Bayonets/Bayonet_VZ58/Bayonet_VZ58_v2.et");
		Attachments.AddItem(1.0, "{1125E1072C3541D3}Prefabs/Weapons/Attachments/Bayonets/Bayonet_VZ58/Bayonet_VZ58_v3.et");

		// TODO: Add suppressors when they release

		return Attachments;	
	}
	
	static WR_WeightedItemArray<ResourceName> GetUtilityItems()
	{
		if (UtilityItems) return UtilityItems;
		UtilityItems = new WR_WeightedItemArray<ResourceName>();

		UtilityItems.AddItem(2.0, "{0CF54B9A85D8E0D4}Prefabs/Items/Equipment/Binoculars/Binoculars_M22/Binoculars_M22.et");
		UtilityItems.AddItem(2.0, "{243948B23D90BECB}Prefabs/Items/Equipment/Binoculars/Binoculars_B8/Binoculars_B8.et");
		UtilityItems.AddItem(2.0, "{F2539FA5706E51E4}Prefabs/Items/Equipment/Binoculars/Binoculars_B12/Binoculars_B12.et");

		UtilityItems.AddItem(1.0, "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et");

		UtilityItems.AddItem(4.0, "{12D5AD21E383B768}Prefabs/Items/Fuel/Jerrycan_01/Jerrycan_01_item.et");
		UtilityItems.AddItem(1.0, "{5028D9E699AE3A73}Prefabs/Items/Equipment/Kits/RearmingKit_01/RearmingKit_01_US.et");
		UtilityItems.AddItem(1.0, "{33B2DFDCD0EBA3DB}Prefabs/Items/Equipment/Kits/RepairKit_01/RepairKit_01_wrench.et");

		UtilityItems.AddItem(2.0, "{CE0AF733722B3978}Prefabs/Items/Equipment/Detonators/BlastingMachine_M34/BlastingMachine_M34.et");
		UtilityItems.AddItem(2.0, "{90976DC90A223095}Prefabs/Items/Equipment/Detonators/BlastingMachine_KPM_3U1/BlastingMachine_KPM_3U1.et");
		
		
		// UtilityItems.AddItem(0.0, "{6D56FED1E55A8F84}Prefabs/Items/Misc/IntelligenceFolder_E_01/IntelligenceFolder_E_01.et");
		// UtilityItems.AddItem(0.0, "{50A68300B537EDFA}Prefabs/Items/Food/ArmyCrackers_Soviet_01.et");
		// UtilityItems.AddItem(0.0, "{C22A2F123CD609FC}Prefabs/Items/Food/Meat_01_Dry.et");
		// UtilityItems.AddItem(0.0, "{128A19AA9F731095}Prefabs/Items/Food/MeatLeg_01.et");
		// UtilityItems.AddItem(0.0, "{EB21D26B8D5A3232}Prefabs/Items/Food/MRE.et");
		// UtilityItems.AddItem(0.0, "{630FB4AD4A735264}Prefabs/Items/Equipment/Canteens/Canteen_US_01.et");
		// UtilityItems.AddItem(0.0, "{3A421547BC29F679}Prefabs/Items/Equipment/Flashlights/Flashlight_MX991/Flashlight_MX991.et");
		// UtilityItems.AddItem(0.0, "{575EA58E67448C2A}Prefabs/Items/Equipment/Flashlights/Flashlight_Soviet_01/Flashlight_Soviet_01.et");
		// UtilityItems.AddItem(0.0, "{78ED4FEF62BBA728}Prefabs/Items/Equipment/Watches/Watch_SandY184A.et");
		// UtilityItems.AddItem(0.0, "{6FD6C96121905202}Prefabs/Items/Equipment/Watches/Watch_Vostok.et");
		// UtilityItems.AddItem(0.0, "{61A705D76908160C}Prefabs/Items/Equipment/Watches/Watch_Orlik38/Watch_Orlik38.et");

		return UtilityItems;	
	}

	static WR_WeightedItemArray<ResourceName> GetMedicalItems()
	{
		if (MedicalItems) return MedicalItems;
		MedicalItems = new WR_WeightedItemArray<ResourceName>();

		MedicalItems.AddItem(4.0, "{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et");
		MedicalItems.AddItem(3.0, "{AF3411AAF94D18C2}Prefabs/Items/Medicine/Gauze_02/Gauze_02.et");
		MedicalItems.AddItem(4.0, "{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et");
		MedicalItems.AddItem(3.0, "{66C501240A143DBF}Prefabs/Items/Medicine/PressureBandage_01/PressureBandage_01.et");
		MedicalItems.AddItem(2.0, "{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et");
		MedicalItems.AddItem(4.0, "{FEF9FA4072013B5B}Prefabs/Items/Medicine/Tourniquet_01/Tourniquet_USSR_01.et");

		return MedicalItems;	
	}

	static WR_WeightedItemArray<ResourceName> GetVests()
	{
		if (Vests) return Vests;
		Vests = new WR_WeightedItemArray<ResourceName>();

		Vests.AddItem(1.0, "{ADE19B33DCBB9005}Prefabs/Characters/Vests/Vest_6B2/Vest_6B2.et");
		Vests.AddItem(1.0, "{4CBDC206FEF9897C}Prefabs/Characters/Vests/Vest_6B3/Vest_6B3.et");
		Vests.AddItem(1.0, "{F396524995F06BFA}Prefabs/Characters/Vests/Vest_SRU21P/Vest_SRU21P.et");
		Vests.AddItem(1.0, "{9713FE6DDCC9510D}Prefabs/Characters/Vests/Vest_Lifchik/Vest_Lifchik.et");
		Vests.AddItem(1.0, "{725C5E1C75CADAF4}Prefabs/Characters/Vests/Vest_M69/Vest_M69_M81woodland.et");
		Vests.AddItem(1.0, "{506EF3B2F0A69D34}Prefabs/Characters/Vests/Vest_NAZ/Vest_NAZ.et");
		Vests.AddItem(1.0, "{4B57C11AA5161760}Prefabs/Characters/Vests/Vest_PASGT/Vest_PASGT.et");
		Vests.AddItem(1.0, "{ED5574EA7F63B457}Prefabs/Characters/Vests/Vest_Type56/Vest_Type56.et");

		return Vests;	
	}

	static WR_WeightedItemArray<ResourceName> GetHelmets()
	{
		if (Helmets) return Helmets;
		Helmets = new WR_WeightedItemArray<ResourceName>();

		Helmets.AddItem(1.0, "{6E14F80C250F85FC}Prefabs/Characters/HeadGear/Helmet_M1_01/Helmet_M1_01.et");
		Helmets.AddItem(1.0, "{F31A715FFDB2BD8E}Prefabs/Characters/HeadGear/Helmet_M1_01/Helmet_M1_01_Cover_Frogskin.et");
		Helmets.AddItem(1.0, "{B74A4FF0DD8BB116}Prefabs/Characters/HeadGear/Helmet_PASGT_01/Helmet_PASGT_01.et");
		Helmets.AddItem(1.0, "{E685A8D337D36204}Prefabs/Characters/HeadGear/Helmet_PASGT_01/Helmet_PASGT_01_cover_w_goggles.et");
		Helmets.AddItem(1.0, "{483A80042765275F}Prefabs/Characters/HeadGear/Helmet_SPH4_01/Helmet_SPH4_01.et");
		Helmets.AddItem(1.0, "{A7E6D7ECD5F684D7}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01.et");
		Helmets.AddItem(1.0, "{5F8928B41FB86990}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01_cover_KZS.et");
		Helmets.AddItem(1.0, "{D4028ED16DEE99EE}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01_camonet.et");
		Helmets.AddItem(1.0, "{E49D9EE7E2B3016C}Prefabs/Characters/HeadGear/Helmet_ZSh5_01/Helmet_ZSh5_01.et");

		return Helmets;	
	}

	static WR_WeightedItemArray<ResourceName> GetHats()
	{
		if (Hats) return Hats;
		Hats = new WR_WeightedItemArray<ResourceName>();

		array<ResourceName> ushankas = {
			"{A22C976003AA5BA3}Prefabs/Characters/HeadGear/Hat_Ushanka_01/Hat_Ushanka_01_folded.et",
			"{323641C91965842E}Prefabs/Characters/HeadGear/Hat_Ushanka_01/Hat_Ushanka_01_unfolded.et",
		};
		Hats.AddItemsDistributedWeight(1.0, ushankas);

		array<ResourceName> boonieHats = {
			"{E12126CDE731EED5}Prefabs/Characters/HeadGear/Hat_Boonie_US_01/Hat_Boonie_US_01.et",
			"{8D75122D5BF561FB}Prefabs/Characters/HeadGear/Hat_Boonie_US_01/Hat_Boonie_US_02.et",
		};
		Hats.AddItemsDistributedWeight(1.0, boonieHats);

		array<ResourceName> flatCaps = {
			"{DD565E2C7E00A5BB}Prefabs/Characters/HeadGear/Hat_FlatCap_01/Hat_FlatCap_01_blue.et",
			"{E97A86BB050ED6D1}Prefabs/Characters/HeadGear/Hat_FlatCap_01/Hat_FlatCap_01_blue_Dirty.et",
			"{67662815EFF734DD}Prefabs/Characters/HeadGear/Hat_FlatCap_01/Hat_FlatCap_01_darkgrey.et",
			"{F6F24096665C6279}Prefabs/Characters/HeadGear/Hat_FlatCap_01/Hat_FlatCap_01_darkgrey_Dirty.et",
			"{6FEB0D9130EC400E}Prefabs/Characters/HeadGear/Hat_FlatCap_01/Hat_FlatCap_01_lightgrey.et",
		};
		Hats.AddItemsDistributedWeight(1.0, flatCaps);

		array<ResourceName> hardHats = {
			"{024D6092DA006DD1}Prefabs/Characters/HeadGear/Hat_Hard_01/Hat_Hard_01_blue.et",
			"{112EF132833109C8}Prefabs/Characters/HeadGear/Hat_Hard_01/Hat_Hard_01_green.et",
			"{DF26044449C06EAB}Prefabs/Characters/HeadGear/Hat_Hard_01/Hat_Hard_01_white.et",
			"{8F5261C6B57B95C8}Prefabs/Characters/HeadGear/Hat_Hard_01/Hat_Hard_01_yellow.et",
		};
		Hats.AddItemsDistributedWeight(1.0, hardHats);

		array<ResourceName> knittedHats = {
			"{BE19A15F1073E7BD}Prefabs/Characters/HeadGear/Hat_Knitted_01/Hat_Knitted_01_black.et",
			"{6B2256EBB8E267DD}Prefabs/Characters/HeadGear/Hat_Knitted_01/Hat_Knitted_01_blue.et",
			"{B9687B8829272E14}Prefabs/Characters/HeadGear/Hat_Knitted_01/Hat_Knitted_01_brown.et",
			"{D8D9EE7F3966589E}Prefabs/Characters/HeadGear/Hat_Knitted_01/Hat_Knitted_01_red.et",
		};
		Hats.AddItemsDistributedWeight(1.0, knittedHats);

		array<ResourceName> m70s = {
			"{B02281435AED8274}Prefabs/Characters/HeadGear/Hat_M70_01/Hat_M70_01.et",
		};
		Hats.AddItemsDistributedWeight(1.0, m70s);

		array<ResourceName> panamaHats = {
			"{793EEE2C133176F9}Prefabs/Characters/HeadGear/Hat_Panama_Soviet/Hat_Panama_Soviet_01_v1.et",
			"{6852E0D026AA8F79}Prefabs/Characters/HeadGear/Hat_Panama_Soviet/Hat_Panama_Soviet_01_v2.et",
		};
		Hats.AddItemsDistributedWeight(1.0, panamaHats);

		array<ResourceName> patrolHats = {
			"{F545C1BE7C99954A}Prefabs/Characters/HeadGear/Hat_Patrol_US_01/Hat_Patrol_US_01.et",
			"{9911F55EC05D1A64}Prefabs/Characters/HeadGear/Hat_Patrol_US_01/Hat_Patrol_US_02.et",
		};
		Hats.AddItemsDistributedWeight(1.0, patrolHats);

		array<ResourceName> peakedCaps = {
			"{3523CB7D5961246B}Prefabs/Characters/HeadGear/Hat_PeakedCap_USSR_01/Hat_PeakedCap_USSR_01.et",
		};
		Hats.AddItemsDistributedWeight(1.0, peakedCaps);

		array<ResourceName> radiovkaBerets = {
			"{C0B4276A1405ED14}Prefabs/Characters/HeadGear/Hat_RadiovkaBeret_01/Hat_RadiovkaBeret_01_beige.et",
			"{200D967D2612147C}Prefabs/Characters/HeadGear/Hat_RadiovkaBeret_01/Hat_RadiovkaBeret_01_black.et",
			"{5D7350496AEA24C8}Prefabs/Characters/HeadGear/Hat_RadiovkaBeret_01/Hat_RadiovkaBeret_01_black_Dirty.et",
			"{6E0EC7AC1F6D748F}Prefabs/Characters/HeadGear/Hat_RadiovkaBeret_01/Hat_RadiovkaBeret_01_brown.et",
			"{75C92359E352391E}Prefabs/Characters/HeadGear/Hat_RadiovkaBeret_01/Hat_RadiovkaBeret_01_brown_Dirty.et",
		};
		Hats.AddItemsDistributedWeight(1.0, radiovkaBerets);

		array<ResourceName> truckerCaps = {
			"{554D761A11D3D7EC}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_sawmillregina.et",
			"{17674905311133D7}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_alsworth.et",
			"{C9886336AAD9F74C}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_enp.et",
			"{6D08415FD18F2775}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_everon.et",
			"{3E2C2A050DE84A64}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_fkdynamo.et",
			"{5977C0A606B65E6D}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_moto.et",
			"{477D52F01DC45C55}Prefabs/Characters/HeadGear/Hat_TruckerCap_01/Hat_TruckerCap_01_pohona.et",
		};
		Hats.AddItemsDistributedWeight(1.0, truckerCaps);

		array<ResourceName> zmijovkaCaps = {
			"{6BCAA9E5BC957693}Prefabs/Characters/HeadGear/Hat_ZmijovkaCap_01/Hat_ZmijovkaCap_01_black.et",
			"{6D225E61B31D513A}Prefabs/Characters/HeadGear/Hat_ZmijovkaCap_01/Hat_ZmijovkaCap_01_green.et",
			"{1B568A254D1E519B}Prefabs/Characters/HeadGear/Hat_ZmijovkaCap_01/Hat_ZmijovkaCap_01_red.et",
		};
		Hats.AddItemsDistributedWeight(1.0, zmijovkaCaps);

		return Hats;	
	}

	static WR_WeightedItemArray<ResourceName> GetBackpacks()
	{
		if (Backpacks) return Backpacks;
		Backpacks = new WR_WeightedItemArray<ResourceName>();

		Backpacks.AddItem(1.0, "{06B68C58B72EAAC6}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium.et");
		Backpacks.AddItem(1.0, "{95D4766BBE46F23D}Prefabs/Items/Equipment/Backpacks/Backpack_IIFS_FieldPack.et");
		Backpacks.AddItem(1.0, "{41A9C55B61F375F0}Prefabs/Items/Equipment/Backpacks/Backpack_Kolobok.et");
		Backpacks.AddItem(1.0, "{FDA7B6630DB87991}Prefabs/Items/Equipment/Backpacks/Backpack_M70_Swiss.et");
		Backpacks.AddItem(1.0, "{4805E67E2AE30F8D}Prefabs/Items/Equipment/Backpacks/Backpack_Medical_M5.et");
		Backpacks.AddItem(1.0, "{3DE0155EC9767B98}Prefabs/Items/Equipment/Backpacks/Backpack_Veshmeshok.et");

		return Backpacks;	
	}
	
	static WR_WeightedItemArray<ResourceName> GetNonReloadableWeapons()
	{
		if (Rifles) return Rifles;
		Rifles = new WR_WeightedItemArray<ResourceName>();

		NonReloadableWeapons.AddItem(1.0, "{9C5C20FB0E01E64F}Prefabs/Weapons/Launchers/M72/Launcher_M72A3.et");

		return Rifles;	
	}
}