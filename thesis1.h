// -----------------------------------------------------------------------------
//
// Copyright (C) 2021 CERN & University of Surrey for the benefit of the
// BioDynaMo collaboration. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#ifndef THESIS1_H_
#define THESIS1_H_

#include "biodynamo.h"
#include "fibroblast.h"

enum Substances { PDGF, TGFb };

namespace bdm {


inline int Simulate(int argc, const char** argv) {
  Simulation simulation(argc, argv);
  
  auto* param = simulation.GetParam();
  auto* myrand = simulation.GetRandom();
  auto* rm = simulation.GetExecutionContext();
  
  //define chemotactic signals
  ModelInitializer::DefineSubstance(PDGF, "PDGF", 0.015, 0.001, 40);
  ModelInitializer::DefineSubstance(TGFb, "TGF-b", 0.005, 0.001, 40);
   
  

  real_t x_coord, y_coord, z_coord;
  
  for (size_t i = 0; i < 10000; ++i) {
    
    //creating random coordinates for agents
    x_coord = myrand->Uniform(param->min_bound, param->max_bound);
    y_coord = myrand->Uniform(param->min_bound, param->max_bound);
    z_coord = myrand->Uniform(param->min_bound, param->max_bound);
    
    
        Cell* platelet = new Cell({x_coord, y_coord, z_coord});             //platelets
        platelet->SetDiameter(1); //μm
    
    //if the random coordinates are outside the V-cut shape, add cells
    if (y_coord < std::abs(-2 * x_coord + 100) + 20 ) { 
    	
    	//above 60 points of y_coord there is the skin epidermis tissue
    	if (y_coord > 60) {
    		Cell* keratinocyte = new Cell({x_coord, y_coord, z_coord}); //keratinocytes -> skin
    		keratinocyte->SetMass(1);
    		keratinocyte->SetDiameter(21); //μm
    		rm->AddAgent(keratinocyte);
    	} 
    	//else below 60points there is the dermis tissue
    	else {
		Cell* neutrophil = new Cell({x_coord, y_coord, z_coord});     //neutrophils
        	neutrophil->AddBehavior(new Chemotaxis("PDGF", 0.19)); 
        	neutrophil->SetDiameter(8.4);
        	rm->AddAgent(neutrophil);   
 
        	Cell* fibroblasts = new Cell({x_coord, y_coord, z_coord});   //fibroblasts
        	fibroblasts->AddBehavior(new Chemotaxis("TGF-b", 0.03));   
        	fibroblasts->SetDiameter(4);
		//fibroblasts->AddBehavior(new GrowthDivision(8,50));
        	rm->AddAgent(fibroblasts);     

		Cell* macrophage = new Cell({x_coord, y_coord, z_coord});   //macrophages
        	macrophage->AddBehavior(new Chemotaxis("PDGF", 0.13));   
        	macrophage->SetDiameter(8.4);
        	macrophage->AddBehavior(new Secretion("TGF-b", 1));
        	rm->AddAgent(macrophage);     
    	
    		if (y_coord - std::abs(-2 * x_coord + 100) + 20 > 25) { //platelets exposed to collagen
        		platelet->AddBehavior(new Secretion("PDGF", 1));  
        		rm->AddAgent(platelet);
        	};	
        
    	}
    	}
    	else {
    	//since blood fills the wound area, the cells of interest (platelets) have to be added in the wound area and release cytokine PDGF
	 	rm->AddAgent(platelet);
	 	platelet->AddBehavior(new Secretion("PDGF", 1));
	 continue;
    		
    	}
                                  
  };
      
      

  // Run simulation for one timestep
  simulation.GetScheduler()->Simulate(1000);

  std::cout << "Simulation completed successfully!" << std::endl;
  return 0;
}


}  // namespace bdm


#endif  // THESIS1_H_
