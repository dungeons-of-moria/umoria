From @MCC.COM:wex@MCC.COM Tue Sep 29 07:05:44 1987
Received: from MCC.COM by ji.Berkeley.EDU (5.58/1.25)
	id AA07225; Tue, 29 Sep 87 07:05:40 PDT
Received: from milano.SW.MCC.COM by MCC.COM with TCP; Tue 29 Sep 87 09:06:24-CDT
Received: from banzai-inst by milano.SW.MCC.COM (5.52/STP1.51) 
	id AA19878; Tue, 29 Sep 87 09:05:12 CDT
Date: Tue, 29 Sep 87 09:04:58 CDT
From: Buckaroo Banzai <wex@MCC.COM>
Message-Id: <8709291404.AA11679@banzai-inst.SW.MCC.COM>
Received: by banzai-inst.SW.MCC.COM (3.2/STP1.14) 
	id AA11679; Tue, 29 Sep 87 09:04:58 CDT
To: wilson@ji.Berkeley.EDU
In-Reply-To: James E. Wilson's message of Mon, 28 Sep 87 20:11:15 PDT <8709290311.AA03868@ji.Berkeley.EDU>
Subject: UNIX Moria
Status: RO

No, I haven't looked at your sources yet.  There are some serious bugs
in the original, which I hope you caught (naturally, I didn't keep a
list of them :-( ).  I, too, corrected the zillions of spelling
mistakes.  Mine doesn't pass lint or run on SYS V.  I haven't decided
how to deal with the rogue-ists who want to use h,j,k,l, etc.

I do have a help program and an extended help library.  I've also made
some changes to make the game better.  EG:

   - you can now recover items from thieving monsters.

   - when monsters drop treasures, they are dropped where the player
   can recover them, instead of inside walls.

   - priests' spells are a little better - they now have
   Word-Of-Recall as a high-level spell.

   - breath weapons, fireballs, etc. now affect everyone in the blast
   radius, including the player and monsters.

   - monsters are now a little smarter about which spells they use (eg
   no fireballs from range 1; no teleport-to-me spells from range 1).

   - a number of useful hints/noises have been added (eg. a
   multiplying monster now goes 'POP'; some potions have helpful
   messages - not giveaways; just enough so that they don't all appear
   to be no-ops).

   - there are error traps and messages (eg lots of places where there
   was no 'default' case on a switch) so that the game doesn't do
   funny things.

   - the shops' inventories have been fixed up so that more and more-
   useful items are available  (shop inventories can now be multiple
   pages instead of just two).

   - there's a Yes/No prompt for accepting a character or re-rolling.

   - the save file is expanded so that the monster list is kept; as it
   was, you could escape a monster by saving and restoring.

   - there was a bug whereby if you were carrying 10 +2,+2 arrows and
   picked up 10 +0,+0 arrows you'd end up with 20 +2,+2 arrows.
   That's fixed.  There was a bug whereby if you ever got too many
   treasures on a level, the game could go into an infinite loop.

There are others, but I can't remember them off the top.  I assume the
moria files are on ji.berkeley.edu?  What directory?

--Alan

From @MCC.COM:wex@MCC.COM Tue Sep 29 12:12:25 1987
Received: from MCC.COM by ji.Berkeley.EDU (5.58/1.25)
	id AA10246; Tue, 29 Sep 87 12:12:21 PDT
Received: from milano.SW.MCC.COM by MCC.COM with TCP; Tue 29 Sep 87 14:12:48-CDT
Received: from banzai-inst by milano.SW.MCC.COM (5.52/STP1.51) 
	id AA22469; Tue, 29 Sep 87 14:08:46 CDT
Date: Tue, 29 Sep 87 14:08:33 CDT
From: Buckaroo Banzai <wex@MCC.COM>
Message-Id: <8709291908.AA12132@banzai-inst.SW.MCC.COM>
Received: by banzai-inst.SW.MCC.COM (3.2/STP1.14) 
	id AA12132; Tue, 29 Sep 87 14:08:33 CDT
To: wilson@ji.Berkeley.EDU
In-Reply-To: James E. Wilson's message of Tue, 29 Sep 87 11:11:15 PDT <8709291811.AA09240@ji.Berkeley.EDU>
Subject: UNIX Moria
Status: RO

In some ways, I've made the game "easier," but I think it's more
balanced.  EG: there's zillions of monsters that drain ST, DX, CO, so
I made elven waybread restore those things.  I also found that priest
had a huge advantage with Sense Invisible, so I made a Ring of See
Invisible available in the Mages' shop.  (Of course, it's still
random, so it doesn't appear most of the time, but there's a chance.)
There are a couple of other minor tweaks like that.

The reroll-character option was the first request I got from my
playtesters.  I sort of agree - some days I want to play a mage and
it's stupid for me to have to quit lots of games.

Another *major* change:  I've improved the scoring algorithm.  It used
to count just gold and exp.  Now I add in a bunch of other things.
The total score is higher, and the overall effect is to reward smart
play (eg if you've spent all your gold to improve your weapon to +4,+4
I now recognize and reward that as if you still had the gold).

We should get together and merge our ideas if possible, no?

--Alan

From @MCC.COM:wex@MCC.COM Wed Sep 30 09:02:55 1987
Received: from MCC.COM by ji.Berkeley.EDU (5.58/1.25)
	id AA21194; Wed, 30 Sep 87 09:02:52 PDT
Received: from milano.SW.MCC.COM by MCC.COM with TCP; Wed 30 Sep 87 11:02:14-CDT
Received: from banzai-inst by milano.SW.MCC.COM (5.52/STP1.51) 
	id AA12165; Wed, 30 Sep 87 11:02:08 CDT
Date: Wed, 30 Sep 87 11:01:54 CDT
From: Buckaroo Banzai <wex@MCC.COM>
Message-Id: <8709301601.AA14128@banzai-inst.SW.MCC.COM>
Received: by banzai-inst.SW.MCC.COM (3.2/STP1.14) 
	id AA14128; Wed, 30 Sep 87 11:01:54 CDT
To: wilson@ji.Berkeley.EDU
In-Reply-To: James E. Wilson's message of Tue, 29 Sep 87 14:05:51 PDT <8709292105.AA11513@ji.Berkeley.EDU>
Subject: UNIX Moria
Status: RO

Got the files transferred and compiled.  You haven't reorganized them,
I see.  I got bloody tired of trying to figure out which routines were
in what file (main, moria1, moria2, misc1, misc2?? - BLEAH) and
restructured the code into more logical modules.  The sizes even out
better, too.

You also seem to have the problem that's been plaguing me - monsters
aren't always visible when they ought to be, especially in dark rooms
and corridors.  I think there's something wrong in draw_block().
Drat!  I was hoping you'd solved that one.

Can I transfer you a copy of my code so you can see what I've done?
I can also ship you the help files and program that runs on them.

Also, how do I get into wizard mode on your moria?  ^p doesn't seem to
do it, as it does on mine.

--Alan


From @MCC.COM:wex@MCC.COM Wed Sep 30 15:19:08 1987
Received: from MCC.COM by ji.Berkeley.EDU (5.58/1.25)
	id AA25694; Wed, 30 Sep 87 15:19:04 PDT
Received: from milano.SW.MCC.COM by MCC.COM with TCP; Wed 30 Sep 87 17:18:31-CDT
Received: from banzai-inst by milano.SW.MCC.COM (5.52/STP1.51) 
	id AA20759; Wed, 30 Sep 87 17:18:27 CDT
Date: Wed, 30 Sep 87 17:18:12 CDT
From: Buckaroo Banzai <wex@MCC.COM>
Message-Id: <8709302218.AA16205@banzai-inst.SW.MCC.COM>
Received: by banzai-inst.SW.MCC.COM (3.2/STP1.14) 
	id AA16205; Wed, 30 Sep 87 17:18:12 CDT
To: wilson@ji.Berkeley.EDU
In-Reply-To: James E. Wilson's message of Wed, 30 Sep 87 11:21:20 PDT <8709301821.AA23038@ji.Berkeley.EDU>
Subject: UNIX Moria
Status: RO

Hokay - the files are in place.  The source file is pretty massive; I
had to segment it into three pieces in order to fit it through our
gateway machine.  Just cat the segments together before unsharing.
It's not compressed or anything.

The best thing to do is copy all the shar files to the directory you
want to install in, then unshar moria.shar first.  There's a README
that explains how to compile and install.  The wizard passwords are a
little hidden (just enough so that someone with the strings program
can't find them easily).  They are DeadlyBalrog and MoriaDungeon.
Capitals count, no carriage return needed.

--Alan

From @MCC.COM:wex@MCC.COM Wed Sep 30 11:35:53 1987
Received: from MCC.COM by ji.Berkeley.EDU (5.58/1.25)
	id AA23167; Wed, 30 Sep 87 11:35:49 PDT
Received: from milano.SW.MCC.COM by MCC.COM with TCP; Wed 30 Sep 87 13:35:31-CDT
Received: from banzai-inst by milano.SW.MCC.COM (5.52/STP1.51) 
	id AA17587; Wed, 30 Sep 87 13:35:26 CDT
Date: Wed, 30 Sep 87 13:35:12 CDT
From: Buckaroo Banzai <wex@MCC.COM>
Message-Id: <8709301835.AA14738@banzai-inst.SW.MCC.COM>
Received: by banzai-inst.SW.MCC.COM (3.2/STP1.14) 
	id AA14738; Wed, 30 Sep 87 13:35:12 CDT
To: wilson@ji.Berkeley.EDU
In-Reply-To: James E. Wilson's message of Wed, 30 Sep 87 11:21:20 PDT <8709301821.AA23038@ji.Berkeley.EDU>
Subject: UNIX Moria
Status: RO

> >You also seem to have the problem that's been plaguing me - monsters
> >aren't always visible when they ought to be, especially in dark rooms
> >and corridors.  I think there's something wrong in draw_block().
> >Drat!  I was hoping you'd solved that one.
> I have never noticed this.  Could you be a little more specific?
> How can I reproduce this?

Go to any dark room (preferrably with a human character so infravision
doesn't mess things up) and wander around.  Sometimes you'll be right
next to a monster but be unable to see it.  It also happens in
corridors, if you bump into a monster unexpectedly.  You get messages
like "You hit it," when you should get messages with the monster's
name.

I've tracked the problem down to the 'ml' field in the monster record.
Y'see, draw_block() redraws the area around the player, but doesn't
always set that field when it should.  I'd like to find a VMS version
to play with and see if it works, but I haven't been able to get the
VMS code I have to run, and I don't know the wizard passwords for it
either :-(

Re: passwords.  Nope.  I just went back and tried again - ^p and ^P
don't ask for the password.  I've compiled it on my Sun3 - have you
tried it on that machine yet?

> >The rogue-command interface is really kludgey - I'm going to hold off
> >installing that in my code for a while.
> Yeah, sort of, but it IS nice if you don't have a numeric keypad.

I guess.  it didn't take me long to adjust, but then I don't play
rogue/hack much anymore.  I'm thinking of including a patch to handle
the Sun's keypad (which generates weird escape-strings).

> The signals are some of the most important code.  I never would have
> gotten any serious playtesters if they lost games due to bugs.
> I had some people fighting the Balrog even with buggy versions of the
> game, because they did not have to start all over again every time the
> program crashed.  (anecdotal evidence: my first game of xconq was lost
> due to a floating point exception, I immediately added signal handlers
> to save games before core dumping, this proved useful during my second game :-)

I guess.  I was too embarrassed to let other people use the code while
it crashed like that.  For myself, I just used wizard mode and dbx to
regenerate games that got blown away by bugs.  Want to ship me your
diffs to xconq?  I'm getting tired of having it core dump on me all
the time.

I will try putting my files on ucbarpa and let you know when they're
there.

--Alan

