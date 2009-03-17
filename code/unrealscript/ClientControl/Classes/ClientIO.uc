
/* WARNING! This file was auto-generated by jpp.  You probably want to be editing ./ClientIO.uc.jpp instead. */



//================================================================================
// ClientIO
//================================================================================

// TODO: Hide mapname (atm it appears in actor's name and weapon's name).
//       Don't send only Pawn info.  Send also projectiles, and local items.  Send all actors?!
//       Send updates instead of everything each time (especially for actors which never move).
class ClientIO expands Actor config (ClientIO);
var config bool bLogAllPawns;
var PlayerPawn OurPlayer;
var int TickCount;
defaultproperties {
bLogAllPawns=true
}
function PostBeginPlay() { // In case spawned rather than selected from menu
 Super.PostBeginPlay();
 // This allows us to "summon ClientIO.ClientIO" in a local game:
 if (OurPlayer==None && PlayerPawn(Owner)!=None) {
  Init(PlayerPawn(Owner));
 } // TODO: Does this work if we were spawned client-side as a mutator?  Maybe we should check Role.
}
// This is the initialisation function
function Init(PlayerPawn o) {
 OurPlayer = o;
 Enable('Tick');
 Log("[ClientIO] Initialised for "$o.getHumanName());
}
event Tick (float DeltaTime) {
 // Log("### State at "$Level.TimeSeconds$" follows after "$DeltaTime);
 SendData("### START STATE");
 SendData("Moment="$TickCount$" Time="$Level.TimeSeconds$" DeltaTime="$DeltaTime);
 // SendData("POV="$OurPlayer);
 TickCount++;
 SendUpdates(DeltaTime);
 SendData("### END STATE");
 Super.Tick(DeltaTime);
}
// event Timer () {
// }
function SendUpdates(float DeltaTime) {
 local Actor A;
 local Pawn P;
 local String n;
 local String pawnExtra;
 SendData("Type ID Name Location Velocity Physics (ViewRotation Weapon Duck Fire Altfire)"); // Jump");
 foreach AllActors(class'Actor',A) {
  pawnExtra = "";
  if (A.IsA('Pawn')) {
   P = Pawn(A);
   if (P.IsA('Bot'))
    n = P.PlayerReplicationInfo.PlayerName; // Needed for bots during online play
   else
    n = P.getHumanName();
   pawnExtra = " "$ P.ViewRotation $" "$ P.Weapon $" "$ P.bDuck $" "$ P.bFire;
  // } else if (A.IsA('Projectile')) {
  } else if (Projectile(A) != None) {
   n = "" $ A.Name;
  } else {
   continue; // We won't report this actor
  }
  SendData(A.class $" "$ A $" "$ A.getHumanName() $" "$ A.Location $" "$ A.Velocity $" "$ A.Physics $ pawnExtra);
  //  $" "$ P.bJump
  // TODO: Show the pawn's mesh (sprite?  and frame?)
 }
}
function bool WeAreOnline() {
 return False;
}
function SendData(String str) {
 Log(str);
}