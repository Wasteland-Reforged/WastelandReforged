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
    private static ref WR_WeightedItemArray<ResourceName> Headgear;
    private static ref WR_WeightedItemArray<ResourceName> Backpacks;
	
	static WR_WeightedItemArray<ResourceName> GetRifles()
	{
		if (Rifles) return Rifles;
		Rifles = new WR_WeightedItemArray<ResourceName>();

		Rifles.AddItem(8.0, "{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et");
		Rifles.AddItem(6.0, "{F97A4AC994231900}Prefabs/Weapons/Rifles/M16/Rifle_M16A2_carbine.et");
		Rifles.AddItem(8.0, "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et");
		Rifles.AddItem(6.0, "{96DFD2E7E63B3386}Prefabs/Weapons/Rifles/AK74/Rifle_AK74N.et");
		Rifles.AddItem(8.0, "{BFEA719491610A45}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74U.et");
		Rifles.AddItem(6.0, "{FA0E25CE35EE945F}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74UN.et");
		Rifles.AddItem(6.0, "{9C948630078D154D}Prefabs/Weapons/Rifles/VZ58/Rifle_VZ58P.et");
		Rifles.AddItem(6.0, "{443CEFF17E040B11}Prefabs/Weapons/Rifles/VZ58/Rifle_VZ58V.et");

		return Rifles;	
	}

	static WR_WeightedItemArray<ResourceName> GetMachineGuns()
	{
		if (MachineGuns) return MachineGuns;
		MachineGuns = new WR_WeightedItemArray<ResourceName>();

		MachineGuns.AddItem(4.0, "{D2B48DEBEF38D7D7}Prefabs/Weapons/MachineGuns/M249/MG_M249.et");
		MachineGuns.AddItem(3.0, "{D182DCDD72BF7E34}Prefabs/Weapons/MachineGuns/M60/MG_M60.et");
		MachineGuns.AddItem(6.0, "{A89BC9D55FFB4CD8}Prefabs/Weapons/MachineGuns/PKM/MG_PKM.et");
		MachineGuns.AddItem(6.0, "{A7AF84C6C58BA3E8}Prefabs/Weapons/MachineGuns/RPK74/MG_RPK74.et");
		MachineGuns.AddItem(6.0, "{5F365605E36597FB}Prefabs/Weapons/MachineGuns/RPK74/MG_RPK74N.et");
		MachineGuns.AddItem(6.0, "{026CE108BFB3EC03}Prefabs/Weapons/MachineGuns/UK59/MG_UK59.et");

		return MachineGuns;	
	}

	static WR_WeightedItemArray<ResourceName> GetSnipers()
	{
		if (Snipers) return Snipers;
		Snipers = new WR_WeightedItemArray<ResourceName>();

		Snipers.AddItem(1.0, "{3EB02CDAD5F23C82}Prefabs/Weapons/Rifles/SVD/Rifle_SVD.et");
		Snipers.AddItem(1.0, "{B31929F65F0D0279}Prefabs/Weapons/Rifles/M14/Rifle_M21.et");

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

		Ordnance.AddItem(5.0, "");
		Ordnance.AddItem(3.0, "");

		return Ordnance;	
	}

	static WR_WeightedItemArray<ResourceName> GetAttachments()
	{
		if (Attachments) return Attachments;
		Attachments = new WR_WeightedItemArray<ResourceName>();

		Attachments.AddItem(4.0, "{08286DDBB1F33FF1}Prefabs/Weapons/Attachments/Optics/Optic_AP2k/Collim_AP2k.et");
		Attachments.AddItem(4.0, "{BD496EE1B40DC510}Prefabs/Weapons/Attachments/Optics/Optic_4x20/Optic_4x20.et");
		Attachments.AddItem(4.0, "{D2018EDB1BBF4C88}Prefabs/Weapons/Attachments/Optics/Optic_ARTII/Optic_ARTII.et");
		Attachments.AddItem(4.0, "{ACDF49FACD0701A8}Prefabs/Weapons/Attachments/Optics/Optic_1P29/Optic_1P29.et");
		Attachments.AddItem(4.0, "{E5E9DBBF3BFB88C6}Prefabs/Weapons/Attachments/Optics/Optic_PGO7/Optic_PGO7V3.et");
		Attachments.AddItem(4.0, "{C850A33226B8F9C1}Prefabs/Weapons/Attachments/Optics/Optic_PSO1/Optic_PSO1.et");
		Attachments.AddItem(6.0, "{4A815EB8B824974A}Prefabs/Weapons/Attachments/Muzzle/FlashHider_AK74/FlashHider_AK74.et");
		Attachments.AddItem(6.0, "{06D4C36A6D585275}Prefabs/Weapons/Attachments/Muzzle/FlashHider_AKS74u/FlashHider_AKS74u.et");
		Attachments.AddItem(6.0, "{6288A1F1A5E3AC37}Prefabs/Weapons/Attachments/Muzzle/FlashHider_M16A2/FlashHider_M16.et");
		Attachments.AddItem(3.0, "{1ABABE3551512B0A}Prefabs/Weapons/Attachments/Underbarrel/UGL_GP25.et");
		Attachments.AddItem(3.0, "{AB268D088F2D6291}Prefabs/Weapons/Attachments/Underbarrel/UGL_M203_short.et");
		// TODO: add bayonets and suppressors when they release

		return Attachments;	
	}
	
	static WR_WeightedItemArray<ResourceName> GetUtilityItems()
	{
		if (UtilityItems) return UtilityItems;
		UtilityItems = new WR_WeightedItemArray<ResourceName>();

		UtilityItems.AddItem(2.0, "{0CF54B9A85D8E0D4}Prefabs/Items/Equipment/Binoculars/Binoculars_M22/Binoculars_M22.et");
		UtilityItems.AddItem(2.0, "{243948B23D90BECB}Prefabs/Items/Equipment/Binoculars/Binoculars_B8/Binoculars_B8.et");
		UtilityItems.AddItem(2.0, "{F2539FA5706E51E4}Prefabs/Items/Equipment/Binoculars/Binoculars_B12/Binoculars_B12.et");
		UtilityItems.AddItem(4.0, "{12D5AD21E383B768}Prefabs/Items/Fuel/Jerrycan_01/Jerrycan_01_item.et");
		UtilityItems.AddItem(1.0, "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et");
		UtilityItems.AddItem(1.0, "{5028D9E699AE3A73}Prefabs/Items/Equipment/Kits/RearmingKit_01/RearmingKit_01_US.et");
		UtilityItems.AddItem(1.0, "{33B2DFDCD0EBA3DB}Prefabs/Items/Equipment/Kits/RepairKit_01/RepairKit_01_wrench.et");
		UtilityItems.AddItem(1.0, "{6D56FED1E55A8F84}Prefabs/Items/Misc/IntelligenceFolder_E_01/IntelligenceFolder_E_01.et");
		UtilityItems.AddItem(2.0, "{50A68300B537EDFA}Prefabs/Items/Food/ArmyCrackers_Soviet_01.et");
		UtilityItems.AddItem(2.0, "{C22A2F123CD609FC}Prefabs/Items/Food/Meat_01_Dry.et");
		UtilityItems.AddItem(2.0, "{128A19AA9F731095}Prefabs/Items/Food/MeatLeg_01.et");
		UtilityItems.AddItem(2.0, "{EB21D26B8D5A3232}Prefabs/Items/Food/MRE.et");
		UtilityItems.AddItem(2.0, "{630FB4AD4A735264}Prefabs/Items/Equipment/Canteens/Canteen_US_01.et");
		UtilityItems.AddItem(2.0, "{3A421547BC29F679}Prefabs/Items/Equipment/Flashlights/Flashlight_MX991/Flashlight_MX991.et");
		UtilityItems.AddItem(2.0, "{575EA58E67448C2A}Prefabs/Items/Equipment/Flashlights/Flashlight_Soviet_01/Flashlight_Soviet_01.et");
		UtilityItems.AddItem(1.0, "{78ED4FEF62BBA728}Prefabs/Items/Equipment/Watches/Watch_SandY184A.et");
		UtilityItems.AddItem(1.0, "{6FD6C96121905202}Prefabs/Items/Equipment/Watches/Watch_Vostok.et");
		UtilityItems.AddItem(1.0, "{61A705D76908160C}Prefabs/Items/Equipment/Watches/Watch_Orlik38/Watch_Orlik38.et");

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

	static WR_WeightedItemArray<ResourceName> GetHeadgear()
	{
		if (Headgear) return Headgear;
		Headgear = new WR_WeightedItemArray<ResourceName>();

		Headgear.AddItem(1.0, "{6E14F80C250F85FC}Prefabs/Characters/HeadGear/Helmet_M1_01/Helmet_M1_01.et");
		Headgear.AddItem(1.0, "{F31A715FFDB2BD8E}Prefabs/Characters/HeadGear/Helmet_M1_01/Helmet_M1_01_Cover_Frogskin.et");
		Headgear.AddItem(1.0, "{B74A4FF0DD8BB116}Prefabs/Characters/HeadGear/Helmet_PASGT_01/Helmet_PASGT_01.et");
		Headgear.AddItem(1.0, "{E685A8D337D36204}Prefabs/Characters/HeadGear/Helmet_PASGT_01/Helmet_PASGT_01_cover_w_goggles.et");
		Headgear.AddItem(1.0, "{483A80042765275F}Prefabs/Characters/HeadGear/Helmet_SPH4_01/Helmet_SPH4_01.et");
		Headgear.AddItem(1.0, "{A7E6D7ECD5F684D7}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01.et");
		Headgear.AddItem(1.0, "{5F8928B41FB86990}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01_cover_KZS.et");
		Headgear.AddItem(1.0, "{D4028ED16DEE99EE}Prefabs/Characters/HeadGear/Helmet_SSh68_01/Helmet_SSh68_01_camonet.et");
		Headgear.AddItem(1.0, "{E49D9EE7E2B3016C}Prefabs/Characters/HeadGear/Helmet_ZSh5_01/Helmet_ZSh5_01.et");
		// TODO: add other hats

		return Headgear;	
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
}