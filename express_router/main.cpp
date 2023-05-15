#include <iostream>

#include <restinio/all.hpp>
#include <restinio/websocket/websocket.hpp>
#include <json_dto/pub.hpp>

namespace rws = restinio::websocket::basic;
namespace rr = restinio::router;


using ws_registry_t = std::map< std::uint64_t, rws::ws_handle_t >;


struct place_t
{
	place_t() = default;

	place_t
	(
		std::string placeName,
		double lat,
		double lon
	)
		:	m_placeName{ std::move( placeName ) }
		,	m_lat{ std::move( lat ) }
		,	m_lon{ std::move( lon ) }
	{}

	template < typename JSON_IO >
	void
	json_io( JSON_IO & io )
	{
		io
			& json_dto::mandatory( "placeName", m_placeName )
			& json_dto::mandatory( "latitude", m_lat )
			& json_dto::mandatory( "longitude", m_lon );
	}
	std::string m_placeName;
	double m_lat;
	double m_lon;
};

struct weatherReport_t
{
	weatherReport_t() = default;

	weatherReport_t
	(
		int id,
		std::string date,
		std::string time,
		place_t place,
		float temperature,
		int humidity
	)
		:	m_id{ std::move( id ) }
		,	m_date{ std::move( date ) }
		,	m_time{ std::move( time ) }
		,	m_place{ std::move( place ) }
		,	m_temperature{ std::move( temperature ) }
		,	m_humidity{ std::move( humidity ) }
	{}

	template < typename JSON_IO >
	void
	json_io( JSON_IO & io )
	{
		io
			& json_dto::mandatory( "id", m_id )
			& json_dto::mandatory( "date", m_date )
			& json_dto::mandatory( "time", m_time )
			& json_dto::mandatory( "place", m_place )
			& json_dto::mandatory( "temperature", m_temperature )
			& json_dto::mandatory( "humidity", m_humidity );
	}
	int m_id;
	std::string m_date;
	std::string m_time;
	place_t m_place;
	float m_temperature;
	int m_humidity;
};

using weatherStation_t = std::vector< weatherReport_t >;


using router_t = rr::express_router_t<>;

using traits_t =
	restinio::traits_t<
	restinio::asio_timer_manager_t,
	restinio::single_threaded_ostream_logger_t,
	router_t >;

class weatherReport_handler_t
{
public :
	explicit weatherReport_handler_t( weatherStation_t & weatherReport )
		:	m_weatherReport_t( weatherReport )
	{}

	weatherReport_handler_t( const weatherReport_handler_t & ) = delete;
	weatherReport_handler_t( weatherReport_handler_t && ) = delete;

	auto on_weatherReport_list(
		const restinio::request_handle_t& req, rr::route_params_t ) const
	{
		auto resp = init_resp( req->create_response() );

		try
		{
			std::vector < weatherReport_t > weather_data;
			for (auto i = m_weatherReport_t.begin(); i != m_weatherReport_t.end(); i++)
			{
				weather_data.push_back(*i);
			}
			resp.set_body(json_dto::to_json(weather_data));
			
		}
		catch(const std::exception& e)
		{
			mark_as_bad_request(resp);
		}
		return resp.done();
	}

	auto on_weatherReport_last3(
		const restinio::request_handle_t& req, rr::route_params_t ) const
	{
		auto resp = init_resp( req->create_response() );


		if(m_weatherReport_t.size() >= 3)
		{
		try
		{
			std::vector < weatherReport_t > weather_data;
			for (auto i = m_weatherReport_t.rbegin()+2; i > m_weatherReport_t.rbegin()-1; i--)
			{
				weather_data.push_back(*i);
			}
			resp.set_body(json_dto::to_json(weather_data));
			
		}
		catch(const std::exception& e)
		{
			mark_as_bad_request(resp);
		}
		}else{
			resp.set_body(
			"Not enough reports.\n" );
		}

		return resp.done();
	}
	auto on_weatherReport_get(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto reportNum = restinio::cast_to< std::uint32_t >( params[ "reportNum" ] );

		auto resp = init_resp( req->create_response() );

		if( 0 != reportNum && reportNum <= m_weatherReport_t.size() )
		{
			const auto & report = m_weatherReport_t[ reportNum - 1 ];
			resp.set_body(
				"Report #" + std::to_string( reportNum ) + "[" + std::to_string(report.m_id) + ", " + report.m_date + ", " + report.m_time + ", " + report.m_place.m_placeName + 
				", " + std::to_string(report.m_place.m_lat) + ", " + std::to_string(report.m_place.m_lon) + ", " + std::to_string(report.m_temperature) + 
				", " + std::to_string(report.m_humidity) +  "]\n" );

		}
		else
		{
			resp.set_body(
				"No weatherReport with #" + std::to_string( reportNum ) + "\n" );
		}

		return resp.done();
	}

	auto on_date_get(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		auto resp = init_resp( req->create_response() );
		try
		{
			auto date = restinio::utils::unescape_percent_encoding( params[ "date" ] );

			resp.set_body( "weatherReport of " + date + ":\n" );
			try
			{
				std::vector < weatherReport_t > weather_data;
				for (unsigned int i = 0; i < m_weatherReport_t.size(); i++)
				{
					const auto & b = m_weatherReport_t[i];
					if (date == b.m_date)
					{
						weather_data.push_back(b);
					}
				}
				resp.set_body(json_dto::to_json(weather_data));
				
			}
			catch(const std::exception& e)
			{
				mark_as_bad_request(resp);
			}
			
		}
		catch( const std::exception & )
		{
			mark_as_bad_request( resp );
		}

		return resp.done();
	}

	auto on_new_weatherReport(
		const restinio::request_handle_t& req, rr::route_params_t )
	{
		auto resp = init_resp( req->create_response() );

		try
		{
			m_weatherReport_t.emplace_back(
				json_dto::from_json< weatherReport_t >( req->body() ) );

			sendMessage("POST: id = " + json_dto::from_json<weatherReport_t>(req->body()).m_id);
		}
		catch( const std::exception & /*ex*/ )
		{
			mark_as_bad_request( resp );
		}

		return resp.done();
	}

	auto on_weatherReport_update(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto reportNum = restinio::cast_to< std::uint32_t >( params[ "reportNum" ] );

		auto resp = init_resp( req->create_response() );

		try
		{
			auto report = json_dto::from_json< weatherReport_t >( req->body() );

			if( 0 != reportNum && reportNum <= m_weatherReport_t.size() )
			{
				m_weatherReport_t[ reportNum - 1 ] = report;
				sendMessage("PUT: id = " + json_dto::from_json<weatherReport_t>(req->body()).m_id); // hvad er meningen??
			}
			else
			{
				mark_as_bad_request( resp );
				resp.set_body( "No weatherReport with #" + std::to_string( reportNum ) + "\n" );
			}
		}
		catch( const std::exception & /*ex*/ )
		{
			mark_as_bad_request( resp );
		}

		return resp.done();
	}

	auto on_weatherReport_delete(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto reportNum = restinio::cast_to< std::uint32_t >( params[ "reportNum" ] );

		auto resp = init_resp( req->create_response() );

		if( 0 != reportNum && reportNum <= m_weatherReport_t.size() )
		{
			const auto & report = m_weatherReport_t[ reportNum - 1 ];
			resp.set_body(
				"Delete weatherReport #" + std::to_string( reportNum ) + ": " + report.m_place.m_placeName 
				+ "[" + report.m_date + ", "+ report.m_time + "]\n");

			m_weatherReport_t.erase( m_weatherReport_t.begin() + ( reportNum - 1 ) );
		}
		else
		{
			resp.set_body(
				"No weatherReport with #" + std::to_string( reportNum ) + "\n" );
		}

		return resp.done();
	}
	auto on_live_update(const restinio::request_handle_t& req,rr::route_params_t params)
	{
		if (restinio::http_connection_header_t::upgrade == req->header().connection())
		{
			auto wsh = rws::upgrade<traits_t>(
				*req, rws::activation_t::immediate,
				[this](auto wsh, auto m)
				{
					if( rws::opcode_t::text_frame == m->opcode() ||
						rws::opcode_t::binary_frame == m->opcode() ||
						rws::opcode_t::continuation_frame == m-> opcode())
					{
						wsh->send_message(*m);
					}
					else if (rws::opcode_t::ping_frame == m->opcode())
					{
						auto resp = *m;
						resp.set_opcode(rws::opcode_t::pong_frame);
						wsh->send_message(resp);
					}
					else if (rws::opcode_t::connection_close_frame == m->opcode())
					{
						m_registry.erase(wsh->connection_id());
					}
				});
			
			m_registry.emplace(wsh->connection_id(), wsh);

			init_resp(req->create_response()).done();

			return restinio::request_accepted();
		
		}
		return restinio::request_rejected();
		
	}
private :
	weatherStation_t & m_weatherReport_t;

	template < typename RESP >
	static RESP
	init_resp( RESP resp )
	{
		resp
			.append_header( "Server", "RESTinio sample server /v.0.6" )
			.append_header_date_field()
			.append_header( "Content-Type", "text/plain; charset=utf-8" )
			.append_header(restinio::http_field::access_control_allow_origin, "*");

		return resp;
	}

	ws_registry_t m_registry;


	void sendMessage(std::string message)
	{
		for(auto [k, v] : m_registry)
			v -> send_message(rws::final_frame, rws::opcode_t::text_frame, message);
	}


	template < typename RESP >
	static void
	mark_as_bad_request( RESP & resp )
	{
		resp.header().status_line( restinio::status_bad_request() );
	}
};

auto server_handler( weatherStation_t & weatherReport_collection )
{
	auto router = std::make_unique< router_t >();
	auto handler = std::make_shared< weatherReport_handler_t >( std::ref(weatherReport_collection) );

	auto by = [&]( auto method ) {
		using namespace std::placeholders;
		return std::bind( method, handler, _1, _2 );
	};

	auto method_not_allowed = []( const auto & req, auto ) {
			return req->create_response( restinio::status_method_not_allowed() )
					.connection_close()
					.done();
		};



	// Handlers for '/' path.
	router->http_get( "/", by( &weatherReport_handler_t::on_weatherReport_list ) );
	router->http_post( "/", by( &weatherReport_handler_t::on_new_weatherReport ) );

	// Disable all other methods for '/'.
	router->add_handler(
			restinio::router::none_of_methods(
					restinio::http_method_get(), restinio::http_method_post() ),
			"/", method_not_allowed );


	router->http_get( "/last3", by( &weatherReport_handler_t::on_weatherReport_last3 ) );
	

	// Disable all other methods for '/last3'.
	router->add_handler(
			restinio::router::none_of_methods(
					restinio::http_method_get()),
			"/last3", method_not_allowed );



	// Handler for '/date/:date' path.
	router->http_get( "/date/:date", by( &weatherReport_handler_t::on_date_get ) );

	// Disable all other methods for '/date/:date'.
	router->add_handler(
			restinio::router::none_of_methods( restinio::http_method_get() ),
			"/date/:date", method_not_allowed );

	// Handlers for '/:reportNum' path.
	router->http_get(
			R"(/:reportNum(\d+))",
			by( &weatherReport_handler_t::on_weatherReport_get ) );
	router->http_put(
			R"(/:reportNum(\d+))",
			by( &weatherReport_handler_t::on_weatherReport_update ) );
	router->http_delete(
			R"(/:reportNum(\d+))",
			by( &weatherReport_handler_t::on_weatherReport_delete ) );

	// Disable all other methods for '/:reportNum'.
	router->add_handler(
			restinio::router::none_of_methods(
					restinio::http_method_get(),
					restinio::http_method_post(),
					restinio::http_method_delete() ),
			R"(/:reportNum(\d+))", method_not_allowed );
	
	// Live update
	router->http_get("/chat", by(&weatherReport_handler_t::on_live_update));

	return router;
}

int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t =
			restinio::traits_t<
				restinio::asio_timer_manager_t,
				restinio::single_threaded_ostream_logger_t,
				router_t >;

		weatherStation_t weatherReport_collection{
			{ 1, "17-04-2023", "13;38",{"aarhus", 13.692, 19.438}, 13.1, 70},
			{ 2, "17-04-2023", "13;45",{"aarhus", 13.692, 19.438}, 18, 80},
			{ 3, "17-04-2023", "13;57",{"aarhus", 13.692, 19.438}, 19.2, 84}
		};

		restinio::run(
			restinio::on_this_thread< traits_t >()
				.address( "localhost" )
				.request_handler( server_handler( weatherReport_collection ) )
				.read_next_http_message_timelimit( 10s )
				.write_http_response_timelimit( 1s )
				.handle_request_timeout( 1s ) );
	}
	catch( const std::exception & ex )
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
