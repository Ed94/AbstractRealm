// Parent
#include "Assertions.hpp"

#include "Meta/Specification.hpp"


namespace Exception
{
	using Dev::ESeverity;


	namespace Notify
	{
		void Throw(String _message)
		{
			Log_Error(_message);
		}

		void NoEntry(String _message)
		{
			if (!_message.empty())
				Log_Error(_message);
		}

		void NoRecursion(String _message)
		{
			if (!_message.empty())
				Log_Error(_message);
		}

		void NoReentry(String _message)
		{
			if (!_message.empty())
				Log_Error(_message);
		}

		void NotImplemented(String _message)
		{
			if (!_message.empty())
				Log_Error(_message);
		}
	}

	namespace Fatal
	{
		using Meta::BuildIsNonTuned;

		void Throw(String _message)
		{
			Log_Error(_message);

			assert(false);
		}

		void NoEntry(String _message)
		{
			Log_Error("Fatal: NoEntry, " + _message);
			
			static_assert(BuildIsNonTuned, "NoEntry Assertion");
		}

		void NoRecursion(String _message)
		{
			Log_Error("Fatal: NoRecursion, " + _message);

			static_assert(BuildIsNonTuned, "NoRecursion Assertion");
		}

		void NoReentry(String _message)
		{
			Log_Error("Fatal: NoReentry, " + _message);

			static_assert(BuildIsNonTuned, "NoReentry Assertion");
		}

		void NotImplemented(String _message)
		{
			Log_Error("Fatal: NotImplemented, " + _message);

			static_assert(BuildIsNonTuned, "NotImplemented Assertion");
		}

		void NulLReference(String _message)
		{
			Log_Error("Fatal: NullReference, " + _message);

			static_assert(BuildIsNonTuned, "NullReference Assertion");
		}
	}
}