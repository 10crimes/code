class sgItemForceGun extends sgaItem;

// TODO/CHECK: Due to its expense and usefulness, the ForceGun should drop when
// player is killed, for teammate or for enemy.  This may be better handled in
// the item class.  The problem is, I want that in Siege games, but there's no
// point in GrapplingCTF games, since everyone has one!

simulated function PostBeginPlay() {
	Super.PostBeginPlay();
	InventoryType = String(class'ForceGun');
}

simulated function OnGive(Pawn Target, Inventory Inv) {
	ForceGun(Inv).PullStrength = ForceGun(Inv).PullStrength * (0.5 + 0.5*Grade/5);
}

defaultproperties {
	// InventoryType="kxForceGun.ForceGun"
	// InventoryClass=class'kxForceGun.ForceGun'
	BuildingName="Force Gun"
	BuildCost=1500
	UpgradeCost=50
	Model=LodMesh'Botpack.ImpPick'
}

