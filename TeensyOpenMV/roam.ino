void Roam()
{
  telem << "Entered Roam mode" << endl;
	while(1){
		if(telem.available() > 0) {
			int val = telem.read();  //read telem input commands  
			if(val == 't') {
				telem.println("Toggle Roam Mode Off"); 
				toggleRoam();
				return;
			}
	  }

    readSensors();
	}
}





