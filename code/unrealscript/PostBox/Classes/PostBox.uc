/** This file was auto-generated by jpp.  You probably want to be editing ./PostBox.uc.jpp instead. **/



class PostBox expands Mutator config(PostBox);

// BUG TODO: if someone has a similar nick to you, and a passworded account, and a message waiting before your message,
//           then you will be unable to clear their message, and unable to read the message intended for you!
// This could even be exploited by a malicious user.  If they create an account for every letter of the alphabet, and post to each of them, nobody else will be able to receive posts!
// Possible solutions: skip to the next message if you don't have the password for the first one; force account/recipient names to be at least 5 chars.

// CONSIDER TODO: add a !search command, to list passworded accounts, so you can find your friend's account



var config bool bAnnounceOnJoin;
var config bool bSuggestReply;
var config bool bSendConfirmationMessage;

var config bool bRecommendPasswording;


var config bool bAcceptSpokenCommands;
var config bool bAcceptMutateCommands;
// var config bool bSwallowSpokenCommands; // Now it is swallowing some commands automatically.

var config String mailFrom[1024];
var config String mailTo[1024];
var config String mailDate[1024];
var config String mailMessage[1024];


var config String accountName[1024];
var config String accountPass[1024];


var int lastPlayerChecked;

defaultproperties {
  bAnnounceOnJoin=True
  bSuggestReply=False // Can be a bit too spammy
  bSendConfirmationMessage=False // Can be a bit too spammy

  bRecommendPasswording=False // Can be a bit too spammy

  bAcceptSpokenCommands=True
  bAcceptMutateCommands=True
  // bSwallowSpokenCommands=True
  lastPlayerChecked=0
}

function PostBeginPlay() {
 // If we were not added as a mutator, but run in some other way (e.g. as a ServerActor), then we need to register as a mutator:
   // Level.Game.BaseMutator.AddMutator(Self);

 // Register to receive spoken messages in MutatorTeamMessage() below:
 if (bAcceptSpokenCommands) {
  Level.Game.RegisterMessageMutator(Self);
 }

 SetTimer(23,True); // 15 was too fast; appeared right after gamestart, along with the zp info
}

// Catch messages from players:
function bool MutatorTeamMessage(Actor Sender, Pawn Receiver, PlayerReplicationInfo PRI, coerce string Msg, name Type, optional bool bBeep) {
 local bool hideMessage;
 local string line;
 hideMessage = False;
 if (StrStartsWith(Msg,"!")) {
  if (Sender == Receiver && Sender.IsA('PlayerPawn')) { // Only process each message once.
   CheckMessage(Mid(Msg,1), PlayerPawn(Sender));
  }
  if (StrStartsWith(Locs(Msg),"!mail") || StrStartsWith(Locs(Msg),"!read") || StrStartsWith(Locs(Msg),"!post") || StrStartsWith(Locs(Msg),"!setpass") || StrStartsWith(Locs(Msg),"!changepass")) {
   // We hide the first two for privacy, and !read to save space in the player's chat area
   hideMessage = True;
  }
 }
 return Super.MutatorTeamMessage(Sender,Receiver,PRI,Msg,Type,bBeep) && (!hideMessage); // || !bSwallowSpokenCommands);
}

function Mutate(String str, PlayerPawn Sender) {
 if (bAcceptMutateCommands) {
  CheckMessage(str, Sender);
 }
 Super.Mutate(str, Sender);
}

// Returns True if the command was recognised.
function bool CheckMessage(String line, PlayerPawn Sender) {
 local int argCount;
 local String args[256];
 local Actor A;
 local String command;
 local int i;

 // Log("PostBox.uc.CheckMessage() ("$Sender$"): "$line$"");
 argcount = SplitString(line," ",args);
 command = args[0];

 if (command ~= "HELP") {
  Sender.ClientMessage("PostBox commands:");
  Sender.ClientMessage("  !help");
  Sender.ClientMessage("  !read");
  Sender.ClientMessage("  !mail/!post <part_of_player's_nick> <message>");

  Sender.ClientMessage("  !setpass <account> <password>");
  Sender.ClientMessage("  !changepass <account> <old_pass> <new_pass>");

  return True;
 }

 if (command ~= "READ") {
  ReadMail(Sender,args[1]);
  return True;
 }

 if (command ~= "MAIL" || command ~= "POST") {
  if (args[1] == "" || args[2] == "") {
   Sender.ClientMessage("Usage: !mail <part_of_player's_nick> <message>");
  } else {
   // Save message args[2..] for args[1] (from Sender)
   PostMail(Sender,args[1],StrAfter(StrAfter(line," ")," "));
  }
  return True;
 }


 if (command ~= "SETPASS") {
  if (args[1] == "" || args[2] == "") {
   Sender.ClientMessage("Usage: !setpass <account_name> <password>");
  } else {
   if (StrContains(squishString(Sender.GetHumanName()),squishString(args[1]))) {
    if (isPassworded(args[1])) {
     Sender.ClientMessage("Account "$args[1]$" is already passworded.  Try: !changepass "$args[1]$" <old_pass> <new_pass>");
    } else {
     for (i=0;i<1024;i++) {
      if (accountName[i]=="") {
       accountName[i] = squishString(args[1]);
       accountPass[i] = squishString(args[2]);
       Sender.ClientMessage("Your account "$accountName[i]$" has been passworded.");
       break;
      }
     }
     // BUG: no handling if account database is full
    }
   } else {
    Sender.ClientMessage("You may not password account "$args[1]$" but you may password account "$squishString(Sender.GetHumanName())$".");
   }
  }
  return True;
 }

 if (command ~= "CHANGEPASS") {
  if (args[1] == "" || args[2] == "" || args[3] == "") {
   Sender.ClientMessage("Usage: !changepass <account_name> <old_pass> <new_pass>");
  } else {
   for (i=0;i<1024;i++) {
    if (accountName[i] ~= args[1]) {
     if (accountPass[i] ~= args[2]) {
      accountPass[i] = args[3];
      Sender.ClientMessage("Your password for account "$accountName[i]$" has been changed.");
     } else {
      Sender.ClientMessage("Wrong password.");
     }
     break;
    }
   }
   // BUG: no handling if account was not found
  }
  return True;
 }


 return False;

}

function PostMail(PlayerPawn Sender, String to, String message) {
 local int i;
 for (i=0; i<1024; i++) {
  if (mailFrom[i] == "" && mailTo[i] == "" && mailDate[i] == "" && mailMessage[i] == "") {
   mailFrom[i] = Sender.GetHumanName();
   to = squishString(to);
   mailTo[i] = to;
   mailDate[i] = GetDate();
   mailMessage[i] = message;

   if (isPassworded(to)) {
    Sender.ClientMessage("Message for passworded account " $ to $ " has been saved.");
   } else {
    Sender.ClientMessage("Message for " $ to $ " has been saved.");
   }



   SaveConfig();
   break;
  }
 }
 // BUG: no handling if message database was full
}

// Removes all non-alphanumeric characters from a string
function String squishString(String str) {
 local String newStr;
 local int i,c;
 // str = Caps(str);
 str = Locs(str);
 newStr = "";
 for (i=0; i<Len(str); i++) {
  c = Asc(Mid(str,i,1));
  // if ( (c>=Asc("A") && c<=Asc("Z")) || (c>=Asc("0") && c<=Asc("9")) ) {
  if ( (c>=Asc("a") && c<=Asc("z")) || (c>=Asc("0") && c<=Asc("9")) ) {
   newStr = newStr $ Chr(c);
  }
 }
 return newStr;
}

event Timer() {
 CheckForNewPlayers();
}

function CheckForNewPlayers() {
 local Pawn p;
 while (Level.Game.CurrentID > lastPlayerChecked) {
  for (p=Level.PawnList; p!=None; p=p.NextPawn) {
   if (p.IsA('PlayerPawn') && p.PlayerReplicationInfo.PlayerID == lastPlayerChecked) {
    ProcessNewPlayer(PlayerPawn(p));
    break;
   }
  }
  lastPlayerChecked++;
 }
}

function ProcessNewPlayer(PlayerPawn p) {
 // Check for new mail for this player:
 if ( ! CheckMailFor(p) ) {
  // Only announce the mutator if they didn't have new mail:
  if (bAnnounceOnJoin) {
   p.ClientMessage("This server is running the PostBox mutator.");
   p.ClientMessage("You can leave messages for other players with the !mail command.");
  }
 }
}

function int FindMailFor(PlayerPawn p) {
 local String squishedName;
 local int i;
 // Check for mail for Sender
 squishedName = squishString(p.GetHumanName());
 for (i=0; i<1024; i++) {
  if (mailTo[i] != "" && StrContains(squishedName,squishString(mailTo[i]))) {
   return i;
  }
 }
 return -1;
}

function bool CheckMailFor(PlayerPawn p) {
 local int i;
 i = FindMailFor(p);
 if (i >= 0) {

  if (isPassworded(mailTo[i])) {
   p.ClientMessage(mailFrom[i]$" has left you a message.  Type !read <password> to read it.");
  } else {
   p.ClientMessage(mailFrom[i]$" has left you a message.  Type !read to read it.");
  }



  return True;
 }
 return False;
}

function ReadMail(PlayerPawn p, String password) {
 local int i;
 i = FindMailFor(p);
 if (i == -1) {
  p.ClientMessage("You have no new mail.");
 } else {

  if (isPassworded(mailTo[i]) && !(getPassword(mailTo[i]) ~= password)) {
   p.ClientMessage("Incorrect password for account "$mailTo[i]$".");
   return;
  }

  // Display message:
  // p.ClientMessage(mailFrom[i] $ " -> " $ mailTo[i] $ " @ " $ mailDate[i] $ ": " $ mailMessage[i]);
  // p.ClientMessage("From " $ mailFrom[i] $ " to " $ mailTo[i] $ " at " $ mailDate[i] $ ": " $ mailMessage[i]);
  p.ClientMessage("From " $ mailFrom[i] $ " to " $ mailTo[i] $ " at " $ mailDate[i] $ ":");
  p.ClientMessage("  " $ mailMessage[i]);
  if (bSuggestReply && !(mailFrom[i] ~= "PostMaster")) {
   p.ClientMessage("Reply with: !mail "$squishString(mailFrom[i])$" <your_message>");
  }

  if (!isPassworded(mailTo[i]) && bRecommendPasswording) {
   p.ClientMessage("You can password the "$mailTo[i]$" account with: !setpass "$mailTo[i]$" <password>");
  }


  if (bSendConfirmationMessage && !(mailFrom[i] ~= "PostMaster")) {
   // Send a confirmation message back to the sender, saying their message was received (and by who).
   mailTo[i] = mailFrom[i];
   mailFrom[i] = "PostMaster";
   mailDate[i] = GetDate();
   // mailMessage[i] = ""$p.GetHumanName()$" received your message \""$mailMessage[i]$"\"";
   mailMessage[i] = ""$p.GetHumanName()$" received your message.";
   // Clear message:
  } else {
   mailFrom[i] = "";
   mailTo[i] = "";
   mailDate[i] = "";
   mailMessage[i] = "";
   // BUG TODO: If we don't shunt any later messages up to fill this gap at i, players may end up receiving messages in non-chronological order.
  }
  SaveConfig();

  // Does this player have more messages?
  CheckMailFor(p);
 }
}


function bool isPassworded(String account) {
 return (getPassword(account) != "");
}

function String getPassword(String account) {
 local int i;
 for (i=0;i<1024;i++) {
  if (accountName[i]~=account) {
   return accountPass[i];
  }
 }
 return "";
}
//===============//
//               //
//  JLib.uc.jpp  //
//               //
//===============//
function int SplitString(String str, String divider, out String parts[256]) {
 // local String parts[256];
 // local array<String> parts;
 local int i,nextSplit;
 i=0;
 while (true) {
  nextSplit = InStr(str,divider);
  if (nextSplit >= 0) {
   // parts.insert(i,1);
   parts[i] = Left(str,nextSplit);
   str = Mid(str,nextSplit+Len(divider));
   i++;
  } else {
   // parts.insert(i,1);
   parts[i] = str;
   i++;
   break;
  }
 }
 // return parts;
 return i;
}
function string GetDate() {
    local string Date, Time;
    Date = Level.Year$"-"$PrePad(Level.Month,"0",2)$"-"$PrePad(Level.Day,"0",2);
    Time = PrePad(Level.Hour,"0",2)$":"$PrePad(Level.Minute,"0",2)$"."$PrePad(Level.Second,"0",2);
    return Date$"-"$Time;
}
function string PrePad(coerce string s, string p, int i) {
    while (Len(s) < i)
        s = p$s;
    return s;
}
function bool StrStartsWith(string s, string x) {
 return (InStr(s,x) == 0);
 // return (Left(s,Len(x)) ~= x);
}
// function bool StrEndsWith(string s, string x) {
 // return (Right(s,Len(x)) ~= x);
// }
function bool StrContains(String s, String x) {
 return (InStr(s,x) > -1);
}
function String StrAfter(String s, String x) {
 local int i;
 i = Instr(s,x);
 return Mid(s,i+Len(x));
}
function string StrAfterLast(string s, string x) {
 local int i;
 i = InStr(s,x);
 if (i == -1) {
  return s;
 }
 while (i != -1) {
  s = Mid(s,i+Len(x));
  i = InStr(s,x);
 }
 return s;
}
function string StrBefore(string s, string x) {
 local int i;
 i = InStr(s,x);
 if (i == -1) {
  return s;
 } else {
  return Left(s,i);
 }
}
function string StrBeforeLast(string s, string x) {
 local int i;
 i = InStrLast(s,x);
 if (i == -1) {
  return s;
 } else {
  return Left(s,i);
 }
}
function int InStrOff(string haystack, string needle, int offset) {
 local int instrRest;
 instrRest = InStr(Mid(haystack,offset),needle);
 if (instrRest == -1) {
  return instrRest;
 } else {
  return offset + instrRest;
 }
}
function int InStrLast(string haystack, string needle) {
 local int pos;
 local int posRest;
 pos = InStr(haystack,needle);
 if (pos == -1) {
  return -1;
 } else {
  posRest = InStrLast(Mid(haystack,pos+Len(needle)),needle);
  if (posRest == -1) {
   return pos;
  } else {
   return pos + Len(needle) + posRest;
  }
 }
}
// Converts a string to lower-case.
function String Locs(String in) {
 local String out;
 local int i;
 local int c;
 out = "";
 for (i=0;i<Len(in);i++) {
  c = Asc(Mid(in,i,1));
  if (c>=65 && c<=90) {
   c = c + 32;
  }
  out = out $ Chr(c);
 }
 return out;
}
