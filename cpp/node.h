#ifndef NODE_H_
#define NODE_H_

#include <Eigen/Sparse>

class Node {

	public:

		// Constructor with id and coordinates
		Node(unsigned id, const std::array<double,3>& coords) : id_{id} {
			for (unsigned i = 0; i < coords.size(); i++) coordinates_[i] = coords.at(i);
		}

		// Destructor
		~Node() = default;

		// Copy constructor
		Node(const Node&) = delete;

		// Assignment operator
		Node& operator=(const Node&) = delete;

		// Move constructor
		Node(Node&&) = delete;

		// Return id of the node
		unsigned id() const { return id_; }

		// Return coordinates of the node
		Eigen::Vector3d coordinates() const { return coordinates_; }

		// Return coordinates in a particular direction
		double coord_at_dir(unsigned dir) const { return coordinates_[dir]; }

		// Return number of connection to the node
		unsigned num_of_connection() const { return num_of_connection_; }

		// Assign hydarulic head at the node
		void set_head(double head) {
			head_ = head;
			ishead_ = true;
		}

		// Return hydraulic head at the node
		double head() const { return head_; }

		// Return head assignment status
		bool ishead() const {return ishead_; }

		// Assign discharge at the node
		void set_discharge(double discharge) {
			discharge_ = discharge;
			isdischarge_ = true;
		}

		// Return discharge at the node
		double discharge() const { return discharge_;}

		// Return discharge assignment status
		bool isdischarge() const { return isdischarge_; }

	private:
		// node id
		unsigned id_{std::numeric_limits<unsigned>::max()};
		// nodal coordinates
		Eigen::Vector3d coordinates_;
		// number of connection to the node
		unsigned num_of_connection_;
		// hydraulic head
		double head_;
		// discharge
		double discharge_;
		// whether head is assigned
		bool ishead_{false};
		// whether discharge is assigned
		bool isdischarge_{false};
};

#endif  // NODE_H_
