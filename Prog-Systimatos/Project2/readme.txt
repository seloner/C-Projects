ΑΜ:11520120049
Η εργασία υλοποιήθηκε σε περιβάλλον ubuntu.Στο στον φακελο paths υπαρχουν καποια δοκιμαστικα txt αρχεια που δοκιμασα καθως δεν ειχαμε στην διαθεση μας τα paths που θα χρησιμοποιησετε.Δεν εχω υλοποιησει το bashscript.
Εχω χρησηιμοποιησει τις δομες απο τις προηγουμενη ασκηση το trie και το την λιστα.
Επειδη χρησιμοποιοω την exec εχω δυο makefiles.Για να μεταγλωτισετε το προγραμμα πρεπει να τρεξετε. make -f makejob και make -f makeworker.
Αν ο χρηστης δωσει μεγαλυτερο αριθμο workers απο τα path τοτε μειωνω τον αριθμο των workers οσο και τα path.
Kαθε worker ανοιγει ολα τα αρχεια στο path/s που του εχουν δωθει και περναει την καθε λεξω σε ενα trie και σε μια λιστα ειναι συνδεδεμενη με τους τελικους κομβους του trie και αποθηκευει τον αριθμο της γραμμης και το path του αρχειου που βρισκεται η λεξη.
Στην search εμφανιζω τον αριθμο της γραμμης που βρισκεται η λεξη ,το path και ΔΕΝ εμφανιζω το περιοχομενο της γραμμης .
Χρησιμοπιοω την fork για να δημιουργησω τις διεραγασιες και μετα μια excec και να τρεξω των κωδικο του worker.