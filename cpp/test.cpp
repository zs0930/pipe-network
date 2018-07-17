#include "catch.hpp"
#include "node.h"

TEST_CASE( "Node is checked", "[Node]" ) {
	Node test = Node(123, {11.2, 22.3, 33.4});

	REQUIRE( test.id() == 123 );
	REQUIRE( test.coordinates() == ( 11.2, 22.3, 33.4 );
	REQUIRE( test.coord_at_dir(1) == 22.3 );
	REQUIRE( test.ishead() == false );
	REQUIRE( test.isdischarge() == false );

	SECTION( "reading and writting hydraulic head" ) {
	    test.set_head(110.1);

	    REQUIRE( test.head() == 110.1 );
	    REQUIRE( test.ishead() == true );
	}

	SECTION( "reading and writting discharge" ) {
	    test.set_discharge(-23.4);

	    REQUIRE( test.discharge() == -23.4 );
	    REQUIRE( test.isdischarge() == true );
	}

	//test return num_of_connection
}
