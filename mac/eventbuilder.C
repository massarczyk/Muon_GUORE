void eventbuilder(void)
{
  
  //variable declaration
  //--------------------

  //files and junk
  TFile input("all.root", "read");
 

  //tree stuff
  double time = 0;
  int eventnumber = 0;
  int detectornumber = 0;
  double energy = 0;
  double Event = 0;
  double Detector=0;
  double Time=0;
  double Energy=0;
  
  fTree->SetBranchAddress("energy", &energy);
  fTree->SetBranchAddress("time", &time);
  fTree->SetBranchAddress("eventnumber", &eventnumber);
  fTree->SetBranchAddress("detectornumber", &detectornumber);

  //The following 2D array is the big cheese, handles all sorting and analysis storage
  //First dimension in array is max number of entries in the input tree,
  //second dimension is total number of variables to be saved in output file.
  
  //A 2D array[x][y] of doubles take x*y*8 bytes of RAM upon initialization
  //May want to change how this array business is handled for very high statistics data
  double event[1000000][4]; //Currently takes 1000000*4*8=32 MB of RAM, which is well within limits
  double entries = fTree->GetEntries();

  cout << entries << endl;
  //comparison variables and iterators
  
  double sortingtemp [4];
  double testtime = -20001;
  int testdetector=-2;
  double i;
  double j;
  int k;
  double L=0; //Why is this a double? Sometimes it's better not to ask questions...
  int m = 0;
  int n = 0;
  //timing variables
  double t1;
  double t2;

  t1=double(clock());
  
  //fill the big cheese with tree stuff
  //-----------------------------------

  i = j = 0;
  for(i=0;i<entries;i++)
    {
      fTree->GetEntry(i);

      event[i][1]=double(eventnumber);
      event[i][2]=double(detectornumber);
      event[i][3]=time;
      event[i][4]=energy;

    }

  input.Close();

  cout << "Input file read and closed." << endl << "Beginning sorting process..." << endl;
  
  //declare output file, tree, and branches
  //---------------------------------------
  
  TFile output("builtevents.root","recreate");
  TTree *Tree = new TTree("Tree","Tree");
  Tree->Branch("Event", &Event,"Event/D");
  Tree->Branch("Detector", &Detector,"Detector/D");
  Tree->Branch("Time", &Time,"Time/D");
  Tree->Branch("Energy", &Energy,"Energy/D");
  
  //Sort the big cheese by detectornumber and time
  //This gets the data ready for the event building, which bunches same-detector,
  //same-G4eventnumber entries together in 20us increments.
  //Tree should already be sorted by eventnumber
  //During Co60 calibration simulation, found rare errors in eventnumber sorting. No idea how.
  //---------------------------------------------------

//sort by detector number
  j=0;
  for(i=0;i<=event[entries-1][1];i++)
    {
      if(i==event[j][1]) //only check non-empty G4event numbers
	{
	  k=j;

	  while(event[k][1]==event[k+1][1])
	    k++;

	  L=j; //to allow j comparison multiple times below
	  k=k-j;//number of successful eventnumber matches
	  for(m=0;m<k;m++) //iterate k times
	    {

	      for(j=L;j<(L+k);j++) //compare neighbors k times
		{

		  if(event[j][2]>event[j+1][2]) //if so, sort
		    {
		      for( n=1;n<5;n++)
			sortingtemp[n]=event[j][n];
		      for( n=1;n<5;n++)
			event[j][n]=event[j+1][n];
		      for( n=1;n<5;n++)
			event[j+1][n]=sortingtemp[n];
		    }//end if so, sort
		}//for(j=L
	      
	    }//for(m=0

	  j++;
	      
	}//if(i==event[...
	
    }//for(i=0;...  //end sort by detector number

  
//sort by time
  j=0;
  for(i=0;i<=event[entries-1][1];i++)
    {
      while(i==event[j][1]) //only check non-empty G4event numbers
	{
	  k=j;

	  while(event[k][2]==event[k+1][2])
	    k++; //while detector numbers are equal

	  L=j; //to allow j comparison multiple times below
	  k=k-j;//number of successful eventnumber matches
	  for(m=0;m<k;m++) //iterate k times
	    {

	      for(j=L;j<(L+k);j++) //compare neighbors k times
		{

		  if(event[j][3]>event[j+1][3]) //if so, sort
		    {
		      for( n=1;n<5;n++)
			sortingtemp[n]=event[j][n];
		      for( n=1;n<5;n++)
			event[j][n]=event[j+1][n];
		      for( n=1;n<5;n++)
			event[j+1][n]=sortingtemp[n];
		    }//end if so, sort
		}//for(j=L
	      
	    }//for(m=0
	    
	  j++;
	      
	}//while(i==event[...
	
    }//for(i=0;...  //end sort by time
  

  //Be very careful with the below for loop when trying anything new.
  //This is just designed to fix a very rare G4event disordering at present
  //If input file's G4events aren't already (almost) all in order, this will ruin the rest of the G4eventnumbers
  for(i=0;i<(entries-1);i++) //fudge event numbers out of order.
    {
      if(event[i][1]>event[i+1][1])
	event[i+1][1]=event[i][1];
    }
  
  
  //make sure everything sorted correctly
  for(i=0;i<entries;i++)
    {
      if(event[i][1]>event[i+1][1])
	{
	  cout <<"Event number sorting error at " << i << endl;
	  for(j=(i-5);j<i+6;j++)
	    cout << event[j][1] << "  " << event[j][2] << "  " << event[j][3] << "  " << event[j][4] << "  " << j << endl;
	}
      
      if(event[i][1]==event[1+1][1]&&event[i][2]>event[i+1][2])
	{
	  cout << "Detector number sorting error at " << i << endl;
	  for(j=(i-5);j<i+6;j++)
	    cout << event[j][1] << "  " << event[j][2] << "  " << event[j][3] << "  " << event[j][4] << "  " << j << endl;	  	
	}
      
      if(event[i][1]==event[i+1][1]&&event[i][2]==event[i+1][2]&&event[i][3]>event[i+1][3])
	{
	  cout << "Time sorting error at " << i << endl;
	  for(j=(i-5);j<i+6;j++)
	    cout << event[j][1] << "  " << event[j][2] << "  " << event[j][3] << "  " << event[j][4] << "  " << j << endl;
 	
	}
  

      cout << endl << "Sorting completed. Beginning event building..." << endl;
    
      }
  
  //Bundle hits with same detector number and event number into 20us increments
  //Note: this is probably overly complicated, but it works for now
  //-------------------------------------------------------------------------------------------------------------------------------------------
   
  i=0;
  j=0;
  k=0;
  L=0;
  
  //Actual event building happens here.
  while(i<=event[entries-1][1])
    {
      
      if(event[j][1]==i) //skip empty G4eventnumbers, no double counting
	{
	  
	  k=j; //after next while loop, k-j+1=multiplicity
	  
	  while(event[k][1]==event[k+1][1])//entries with same G4eventnumber
	    { //I DO NOT have to store the G4eventnumber in the resulting data
	      //We'll know the events are separate by virtue of them being
	      //Stored in separate containers
	      k++;
	    }//while(event[k]...

	  while(j<=k&&j<(entries-1)) //Here it is. The place to store events til writing.
	    {
	      
	      if(-1>=(event[j][3]-testtime) || (event[j][3]-testtime)>=20000 )
		testtime = event[j][3]; //If the time was the cause of the break in the event loop below, reset test time
	      if(event[j][2]!=testdetector)
		testdetector = event[j][2]; //If the detector number was the cause of the break in the event loop below, reset test event number
	      
	    
	      
	      while(-1<=(event[j][3]-testtime) && (event[j][3]-testtime)<=20000 && j<(entries-1) && event[j][2]==testdetector) //while is the same G4event, same detectornumber, and within 20us of first time entry
		{

		  //STORE
		  Event=L;
		   Detector=event[j][2];
		  Time=event[j][3];
		  Energy=event[j][4];
		  Tree->Fill();
		  if(int(j)%1000==0)
		    cout << "Event " << j << " filled" << endl;
		  j++;
		}//while(-1<=...
	      	      
	      L++;
	    }//while(j<=k)&&...

	}//if(event[j]...
      
      i++;
      
    }//while(i<=event[...


  //Write output
  Tree->Write();
  output.Close();

  //Report the time this macro took to complete
  t2=clock();
  float diff = ((float)t2-(float)t1);

  cout << endl << endl <<"Time to completion: " << diff/1000000 << " seconds." << endl << endl;
  
}
