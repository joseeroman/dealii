// ---------------------------------------------------------------------
//
// Copyright (C) 2003 - 2018 by the deal.II authors
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

// tests DataOut with multiple DoFHandler objects (vector-valued and scalar
// element)

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>

#include <deal.II/lac/sparsity_pattern.h>

#include <deal.II/numerics/data_out.h>

#include "../tests.h"



template <int dim>
void
test()
{
  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria, 0., 1.);
  tria.refine_global(1);
  tria.begin_active()->set_refine_flag();
  tria.execute_coarsening_and_refinement();

  FE_Q<dim>     fe1(1);
  FESystem<dim> fe2(FE_Q<dim>(1), dim);

  DoFHandler<dim> dof1(tria);
  DoFHandler<dim> dof2(tria);
  dof1.distribute_dofs(fe1);
  dof2.distribute_dofs(fe2);

  Vector<double> v1(dof1.n_dofs()), v2(dof2.n_dofs());
  for (unsigned int i = 0; i < v1.size(); ++i)
    v1(i) = i;
  for (unsigned int i = 0; i < v2.size(); ++i)
    v2(i) = i;

  DataOut<dim> data_out;
  data_out.add_data_vector(dof1, v1, "scalar");
  std::vector<DataComponentInterpretation::DataComponentInterpretation>
    component_interpretation(
      dim, DataComponentInterpretation::component_is_part_of_vector);
  data_out.add_data_vector(dof2,
                           v2,
                           std::vector<std::string>(dim, "vector"),
                           component_interpretation);
  data_out.build_patches();

  data_out.write_vtk(deallog.get_file_stream());
}


int
main()
{
  try
    {
      std::ofstream logfile("output");
      deallog << std::setprecision(2);
      logfile << std::setprecision(2);
      deallog.attach(logfile);

      test<1>();
      test<2>();
      test<3>();

      return 0;
    }
  catch (std::exception &exc)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Exception on processing: " << std::endl
              << exc.what() << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;
      return 1;
    }
  catch (...)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Unknown exception!" << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;
      return 1;
    };
}
