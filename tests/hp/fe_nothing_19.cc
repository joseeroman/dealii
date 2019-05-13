// ---------------------------------------------------------------------
//
// Copyright (C) 1998 - 2019 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------


// a redux of fe_nothing_18

#include <deal.II/base/function.h>

#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_nothing.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h> //standard functions to generate grid
#include <deal.II/grid/tria.h>           //triangulation class

#include <deal.II/hp/dof_handler.h>
#include <deal.II/hp/q_collection.h>

#include <iostream>

#include "../tests.h"

template <int dim>
void
test()
{
  Triangulation<dim> triangulation;
  GridGenerator::hyper_cube(triangulation, -1, 0);
  triangulation.refine_global(1);

  FESystem<dim> elasticity_fe(FE_Q<dim>(1), 1, FE_Nothing<dim>(), 1);
  FESystem<dim> elasticity_w_lagrange_fe(FE_Q<dim>(1), 1, FE_Q<dim>(1), 1);
  hp::FECollection<dim> fe;
  fe.push_back(elasticity_fe);
  fe.push_back(elasticity_w_lagrange_fe);

  hp::DoFHandler<dim>                                dof_handler(triangulation);
  typename hp::DoFHandler<dim>::active_cell_iterator cell =
    dof_handler.begin_active();
  cell->set_active_fe_index(0);
  (++cell)->set_active_fe_index(1);

  dof_handler.distribute_dofs(fe);

  deallog << "n_dofs=" << dof_handler.n_dofs() << std::endl;
  for (auto &cell : dof_handler.active_cell_iterators())
    {
      deallog << cell << ": ";
      std::vector<types::global_dof_index> dof_indices(
        cell->get_fe().dofs_per_cell);
      cell->get_dof_indices(dof_indices);
      for (auto i : dof_indices)
        deallog << i << ' ';
      deallog << std::endl;
    }

  AffineConstraints<double> hanging_node_constraints;
  DoFTools::make_hanging_node_constraints(dof_handler,
                                          hanging_node_constraints);
  hanging_node_constraints.close();

  // print constraints. there shouldn't be any
  deallog << "n_constraints: " << hanging_node_constraints.n_constraints()
          << std::endl;
  hanging_node_constraints.print(deallog.get_file_stream());
}


int
main()
{
  initlog();

  test<1>();
  test<2>();
  test<3>();
}
