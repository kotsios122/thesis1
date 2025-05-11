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

#ifndef fibroblast_H_
#define fibroblast_H_

#include <biodynamo.h>

namespace bdm {

// Define my custom cell MyCell, which extends Cell by adding extra data
// members: cell_color and can_divide
class fibroblast : public Cell {  // our object extends the Cell object
                              // create the header with our new data member
  BDM_AGENT_HEADER(fibroblast, Cell, 1);

 public:
  fibroblast() {}
  explicit fibroblast(const Real3& position) : Base(position) {}
  virtual ~fibroblast() {}

  /// If fibro divides, the daughter has to initialize its attributes
  void Initialize(const NewAgentEvent& event) override {
    Base::Initialize(event);

    if (auto* mother = dynamic_cast<fibroblast*>(event.existing_agent)) {
      cell_color_ = mother->cell_color_;
      if (event.GetUid() == CellDivisionEvent::kUid) {
        // the daughter will be able to divide
        can_divide_ = true;
      } else {
        can_divide_ = mother->can_divide_;
      }
    }
  }

  // getter and setter for our new data member
  void SetCanDivide(bool d) { can_divide_ = d; }
  bool GetCanDivide() const { return can_divide_; }

  void SetCellColor(int cell_color) { cell_color_ = cell_color; }
  int GetCellColor() const { return cell_color_; }

 private:
  // declare new data member and define their type
  // private data can only be accessed by public function and not directly
  bool can_divide_;
  int cell_color_;
};

// Define growth behaviour
struct Growth : public Behavior {
  BDM_BEHAVIOR_HEADER(Growth, Behavior, 1);

  Growth() { AlwaysCopyToNew(); }
  virtual ~Growth() {}

  void Run(Agent* agent) override {
    if (auto* cell = dynamic_cast<fibroblast*>(agent)) {
      if (cell->GetDiameter() < 4) {

        auto* random = Simulation::GetActive()->GetRandom();
        // Here 400 is the speed and the change to the volume is based on the
        // simulation time step.
        // The default here is 0.01 for timestep, not 1.
        cell->ChangeVolume(100);

        // create an array of 3 random numbers between -2 and 2
        Real3 cell_movements = random->template UniformArray<3>(-2, 2);
        // update the cell mass location, ie move the cell
        cell->UpdatePosition(cell_movements);
      } else {  //
        auto* random = Simulation::GetActive()->GetRandom();

        if (cell->GetCanDivide() && random->Uniform(0, 1) < 0.8) {
          cell->Divide();
        } else {
          cell->SetCanDivide(false);  // this cell won't divide anymore
        }
      }
    }
  }
};
}  // namespace bdm


#endif  // _H_
