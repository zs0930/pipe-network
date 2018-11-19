#include "catch.hpp"

#include "matrix_assembler.h"
#include "mesh.h"
#include "node.h"
#include "pipe.h"
#include "settings.h"

// Check matrix_assembler class
TEST_CASE("MatrixAssembler is checked", "[MatrixAssembler]") {
  // Tolerance
  const double tolerance = 1.e-12;

  // Mesh index
  const unsigned meshid = 101;

  // Creat a mesh
  auto mesh = std::make_shared<Mesh>(meshid);

  // Nodal coordinates
  // using example netwrok in Todini(2013)
  const Eigen::Vector3d coords1(1.0, 3.0, 0.0);
  const Eigen::Vector3d coords2(0.0, 2.0, 0.0);
  const Eigen::Vector3d coords3(2.0, 2.0, 0.0);
  const Eigen::Vector3d coords4(0.0, 0.0, 0.0);
  const Eigen::Vector3d coords5(2.0, 0.0, 0.0);
  std::vector<Eigen::Vector3d> coords = {coords1, coords2, coords3, coords4,
                                         coords5};

  // Create nodal pointers based on nodal coordinates in the mesh
  mesh->create_nodes(coords);

  // Make pairs of nodes to create pipe
  std::vector<std::pair<Index, Index>> node_pairs;
  node_pairs.emplace_back(std::make_pair(0, 1));
  node_pairs.emplace_back(std::make_pair(0, 2));
  node_pairs.emplace_back(std::make_pair(1, 2));
  node_pairs.emplace_back(std::make_pair(1, 3));
  node_pairs.emplace_back(std::make_pair(1, 4));
  node_pairs.emplace_back(std::make_pair(2, 4));
  node_pairs.emplace_back(std::make_pair(3, 4));

  // Input vector of pipe diameter and status
  std::vector<double> diameter{1, 1, 1, 1, 1, 1, 1};
  std::vector<bool> status{true, true, true, true, true, true, true};

  // Create pipes based on pipe indices and previous created node pointers in
  // the mesh
  bool all_pipe_created = mesh->create_pipes(node_pairs, diameter, status);

  // Generate vector of nodal and pipe pointers from mesh
  std::vector<std::shared_ptr<pipenetwork::Node>> nodeptr = mesh->nodeptr();
  std::vector<std::shared_ptr<pipenetwork::Pipe>> pipeptr = mesh->pipeptr();

  // Assign initial nodal head and discharge
  double default_init_node_head = 0.0;
  double default_init_node_discharge = 0.0;
  double default_init_pipe_discharge = 0.001;
  double head1 = 100.0;
  double head2 = 99.0;
  double discharge1 = 100.0;
  double discharge2 = -10.0;
  nodeptr.at(0)->head(head1);
  nodeptr.at(0)->discharge(discharge1);
  nodeptr.at(1)->head(head2);
  nodeptr.at(1)->discharge(discharge2);

  // Initialize matrix assembler and assign global index to nodes and pipes
  auto assembler = std::make_shared<MatrixAssembler>();
  // MatrixAssembler assembler = MatrixAssembler();
  assembler->assign_global_nodal_indices(nodeptr);
  assembler->assign_global_pipe_indices(pipeptr);

  // Check the number of nodes and pipes in the network
  unsigned nnodes = assembler->nnodes();
  unsigned npipes = assembler->npipes();
  REQUIRE(nnodes == 5);
  REQUIRE(npipes == 7);

  // Initialize nodal head discharge and pipe discharge vector
  assembler->assemble_node_head_vector();
  assembler->assemble_node_discharge_vector();
  assembler->assemble_pipe_discharge_vector();

  // Check initialized nodal head vector
  Eigen::VectorXd node_head_vec = assembler->node_head_vec();
  REQUIRE(node_head_vec(0) == Approx(head1).epsilon(tolerance));
  REQUIRE(node_head_vec(1) == Approx(head2).epsilon(tolerance));
  for (int i = node_head_vec.size() - 3; i <= node_head_vec.size() - 1; i++)
    REQUIRE(node_head_vec(i) ==
            Approx(default_init_node_head).epsilon(tolerance));

  // Check initialized nodal discharge vector
  Eigen::VectorXd node_discharge_vec = assembler->node_discharge_vec();
  REQUIRE(node_discharge_vec(0) == Approx(discharge1).epsilon(tolerance));
  REQUIRE(node_discharge_vec(1) == Approx(discharge2).epsilon(tolerance));
  for (int i = node_discharge_vec.size() - 3;
       i <= node_discharge_vec.size() - 1; i++)
    REQUIRE(node_discharge_vec(i) ==
            Approx(default_init_node_discharge).epsilon(tolerance));

  // Check initialized pipe discharge vector
  Eigen::VectorXd pipe_discharge_vec = assembler->pipe_discharge_vec();
  for (int i = 0; i <= pipe_discharge_vec.size() - 1; i++)
    REQUIRE(pipe_discharge_vec(i) ==
            Approx(default_init_pipe_discharge).epsilon(tolerance));

  // Assemble Jacobian matrix and check initialized Jacobian matrix
  assembler->assemble_jacobian();
  Eigen::SparseMatrix<double> jac = assembler->jac();
  // Check jacB (nodal discharge in nodal balance equation)
  for (int row = 0; row < nnodes; row++) {
    for (int col = nnodes; col < nnodes + nnodes; col++) {
      if ((col - nnodes) == row) {
        REQUIRE(jac.coeff(row, col) == -1);
      } else {
        REQUIRE(jac.coeff(row, col) == 0);
      }
    }
  }
  // Check jacC (pipe discharge in nodal balance equation)
  REQUIRE(jac.coeff(0, 10) == -1);
  REQUIRE(jac.coeff(0, 11) == -1);
  REQUIRE(jac.coeff(1, 10) == 1);
  REQUIRE(jac.coeff(1, 12) == -1);
  REQUIRE(jac.coeff(1, 13) == -1);
  REQUIRE(jac.coeff(1, 14) == -1);
  REQUIRE(jac.coeff(2, 11) == 1);
  REQUIRE(jac.coeff(2, 12) == 1);
  REQUIRE(jac.coeff(2, 15) == -1);
  REQUIRE(jac.coeff(3, 13) == 1);
  REQUIRE(jac.coeff(3, 16) == -1);
  REQUIRE(jac.coeff(4, 14) == 1);
  REQUIRE(jac.coeff(4, 15) == 1);
  REQUIRE(jac.coeff(4, 16) == 1);
}
