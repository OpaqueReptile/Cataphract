#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <functional>
#include <asio.hpp>
#include "message.h"
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS


using asio::ip::udp;
using asio::ip::tcp;
using namespace std;

//----------------------------------------------------------------------

typedef std::deque<message> message_queue;

//----------------------------------------------------------------------

class peer_participant
{
public:
	virtual ~peer_participant() {}
	virtual void deliver(const message& msg) = 0;
};

typedef std::shared_ptr<peer_participant> peer_participant_ptr;

//----------------------------------------------------------------------

class peer_pool
{
public:
	void join(peer_participant_ptr participant)
	{
		participants_.insert(participant);
		for (auto msg : recent_msgs_)
			participant->deliver(msg);
	}

	void leave(peer_participant_ptr participant)
	{
		participants_.erase(participant);
	}

	void deliver(const message& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}

private:
	std::set<peer_participant_ptr> participants_;
	enum { max_recent_msgs = 100 };
	message_queue recent_msgs_;
};

//----------------------------------------------------------------------
/**
class peer_session : public peer_participant,
	public std::enable_shared_from_this<peer_session>
{
public:
	peer_session(udp::socket socket, peer_pool& room) : socket_(std::move(socket)), room_(room)
	{

	}

	void start()
	{
		room_.join(shared_from_this());
		do_read_header();
	}

	void deliver(const message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	}

private:
	void do_read_header()
	{
		auto self(shared_from_this());
		asio::async_read(socket_,
			asio::buffer(read_msg_.data(), message::header_length),
			[this, self](std::error_code ec, std::size_t )
		{
			if (!ec && read_msg_.decode_header())
			{
				do_read_body();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_read_body()
	{
		auto self(shared_from_this());
		asio::async_read(socket_,
			asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this, self](std::error_code ec, std::size_t )
		{
			if (!ec)
			{
				room_.deliver(read_msg_);
				do_read_header();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_write()
	{
		auto self(shared_from_this());
		asio::async_write(socket_,
			asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
			[this, self](std::error_code ec, std::size_t )
		{
			if (!ec)
			{
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
				{
					do_write();
				}
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}
	udp::socket socket_;
	peer_pool& room_;
	message read_msg_;
	message_queue write_msgs_;
};
**/
//----------------------------------------------------------------------

class server
{
public:
	server(asio::io_service& io_service,
		const udp::endpoint& endpoint)
		:socket_(io_service, endpoint)
	{
		start_receive();
	}

private:
	void start_receive()
	{
		/**
		socket_.async_receive_from(asio::buffer(buf),
			remote_endpoint_,
			std::bind(&server::receive_handler,this,std::placeholders::_1, std::placeholders::_2));
			**/

	}
	void receive_handler(std::error_code ec, std::size_t bytes_transferred)
	{
			if (!ec)
			{
				//std::make_shared<peer_session>(std::move(socket_), room_)->start();

			}
		start_receive();

	}
	
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	peer_pool room_;
	char buf[1];
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		asio::io_service io_service;

		std::list<server> servers;
		for (int i = 1; i < argc; ++i)
		{
			udp::endpoint endpoint(udp::v4(), std::atoi(argv[i]));
			servers.emplace_back(io_service, endpoint);
		}

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
