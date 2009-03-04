/* WARNING! This file was auto-generated by jpp.  You probably want to be editing ./sgBuildOptionAdder.uc.jpp instead. */


class sgBuildOptionAdder expands Inventory;

// Until we sort out a better config:
var globalconfig bool bAddGrappleGun;
var globalconfig bool bAddDoubleJumpBoots;
var globalconfig bool bAddForceGun;

// Example config:
// ExtraItems(1)=ExtraItem(Name="A Bucket",Mesh="BucketMesh",InventoryType="Botpack.Bucket",BuildCost=250,UpgradeCost=50)
// Although this would allow us to add ANY item, it does not give us fine control over what happens when the item is upgraded!
// Mmmm unfortunately the current system requires that a separate sgItem class exists for each item.

defaultproperties {
 bAddGrappleGun=True
 bAddDoubleJumpBoots=True
 bAddForceGun=False
}

simulated function PostBeginPlay() {
 Log(Self$" - calling AddExtraBuildOptions()");
 AddExtraBuildOptions();
 Super.PostBeginPlay();
}

simulated function AddExtraBuildOptions() {
 //// TODO: Remove Invisibility.  It sucks for sgCTF and sgAS.
 //// TODO: Remove Jetpack from sgAS, and from sgCTF until we can force flagdrop when jetting.
 // if ( ! sgCategoryTeleport.class.default.Selections[4] == class'sgItemGrappleGun' ) {
 // InsertInCategory(class'sgCategoryTeleport',class'sgItemGrappleGun',4);
 // }
 if (bAddDoubleJumpBoots)
  InsertInCategory(class'sgCategoryItems',class'sgItemDoubleJumpBoots',2);
 // InsertInCategory(class'sgCategoryItems',class'sgItemGhost',3);
 if (bAddForceGun)
  InsertInCategory(class'sgCategoryItems',class'sgItemForceGun',5);
 // InsertInCategory(class'sgCategoryItems',class'sgItemSpawner',6); // not an item, just a building!  it doesn't display properly in constructor menu ... maybe if we set its defaults?  it doesn't work anyway :P
 if (bAddGrappleGun)
  InsertInCategory(class'sgCategoryTeleport',class'sgItemGrappleGun',4);
 // InsertInCategory(class'sgCategoryExplosives',class'sgItemVoiceBox',5); // Was in 2e but not 2g.
}

simulated function InsertInCategory(class<sgBuildCategory> categoryClass, class<sgBuilding> BuildClass, int pos) {
 local int i;
 if (categoryClass.default.Selections[pos] == BuildClass)
  return; // It's already there where we wanted to put it - do nothing.
 for (i=categoryClass.default.NumSelections;i>pos;i--) {
  categoryClass.default.Selections[i] = categoryClass.default.Selections[i-1];
 }
 categoryClass.default.Selections[pos] = BuildClass;
 // Fixes bug where empty selections appear at the end.
 // Probably due to NumSelections being replicated but Selections[] array not.
 if (categoryClass.default.Selections[categoryClass.default.NumSelections] != None)
  categoryClass.default.NumSelections++;
}
