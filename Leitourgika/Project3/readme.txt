﻿Γιώργος Καλογερόπουλος ΑΜ: 1115201200049
Η εργασία υλοποιήθηκε σε περιβάλλον ubuntu.
Η διεργασία myport δημιουργεί όλες τις διεργασίες καθως και κάνει τις απαραίτητες αρχικοποιήσεις.
Ακόμα παίρνει σαν όρισμα -l το όνομα του αρχείου και -t τον χρόνο που θέλουμε να είναι ενεργό το λιμάνι.
Δεν έχει υλοιποιηθεί η σημαία -u για τα upgrade στα vessels.
Ο port master καλείτε με ./port-master",filename,sharedsizestring
Όπου filename το όνομα του config και sharedsizestring το μέγεθος της shared memory.
Τα κλειδιά για το shmid βρίσκονται default ορισμένα σε μια τιμή.
Ο port master περιμένει να έρθει κάποιο πλοίο στο λιμάνι και είναι υπεύθυνος για την κίνηση στο λιμάνι.Είναι αυτός που ελέγχει τον σεμαφόρο για τις ουρές.Έχω τρεις ουρές αναμονής (τρεις sems)
οι οποίοι έχουν αρχική τιμή τον αριθμό των vessels που εξυπηρετεί η κάθε ουρά.
Μόλις κάποιο vessel περάσει αυτόν τον έλεγχο της ουράς περίμενει σε έναν σεμαφόρο για να πάρει access να κινηθεί στην θέση του στο λιμάνι.H επικοινωνία για την αλλαγή αυτών των πληροφοριών στην είσοδο γίνεται με δύο σεμαφόρους.Ένα για το vessel και ένα για το myport.
Η έξοδος απο το λιμάνι γίνεται με παρόμοια λογική.
Η καταστάση του λιμανιού ουσιαστικά είναι ένα array από vessels ,που έχει s θέσεις,m θέσεις,l θέσεις,
Tα vessels δημιουργούνται ./vessel",sharedsizestring,"-t","m","-m","2","-p","3"
πχ για τύπου m.Πρώτο όρισμα πρέπει να είναι το shared memory size.
Tέλος το monitor παρέχει τα στατιστικά και δημιουργείται 
./monitor","-s",sharedsizestring,"-f",totalslotstring,"-d","120","-t","20"
όπου  sharedsizestring το shared memory size , totalslotstring το μέγιστο αριθμό vessels που εξυπηρετεί το λιμάνι και -d τον χρόνο λειτουργίας του monitor και -t την περιοδικότητα που εκτυπώνει τα στατιστικά.


 