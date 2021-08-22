#pragma once


#include "Exceptions_Backend.hpp"


namespace Exception
{
	namespace Notify
	{
		void Throw(String _message);

		void NoEntry       (String _message);
		void NoRecursion   (String _message);
		void NoReentry     (String _message);
		void NotImplemented(String _message);
	}

	namespace Fatal
	{
		void Throw(String _message);

		void NoEntry       (String _message);
		void NoRecursion   (String _message);
		void NoReentry     (String _message);
		void NotImplemented(String _message);

		void NullReference(String _message);
	}
}
