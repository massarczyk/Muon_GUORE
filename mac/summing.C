void summing(void)
{

  //This macro takes the output of eventbuilder.C as an input and fills a histogram
  //with the summed energies of every built event in that file, then draws it

  
  //variable declaration
  //--------------------

  //files and junk
  TFile input("builtevents.root","read");

  
  //branchy stuff
  double Energy;
  double Event;
  double energysum;
  double entries = Tree->GetEntries();
  //iterators
  double i;
  double j;
  double k;

  cout << entries << endl << endl;
  
  Tree->SetBranchAddress("Energy", &Energy);
  Tree->SetBranchAddress("Event", &Event);
  
  //histogram to be drawn. Might want to make this more flexible some time
  TH1D *hsum = new TH1D("hsum","Summed Energies of All Events",10000, 0, 10000);

  while(i<entries)
    {

      Tree->GetEntry(i); 
      j = Event;
      energysum = Energy;

      i++;
      
      Tree->GetEntry(i);
      k = Event;

      //using a while loop here AFTER iterating i protects against problems caused by events
      //with only one entry
      while(j==k&&i<entries)
	{

	  energysum = energysum + Energy;
	  i++;
	  Tree->GetEntry(i); 
	  j=k;
	  k=Event;
	}//while(j==k)

      //fill histogram here
      hsum->Fill(energysum, 1);

      
    }//while(i<(entries-2))      

  //write here
  cout << "HERE" << endl;
  hsum->SetDirectory(gROOT);
  hsum->Draw();
}
