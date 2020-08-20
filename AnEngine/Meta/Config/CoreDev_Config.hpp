#pragma once







namespace Meta
{
	/**
	 * See: https://twitter.com/TimSweeneyEpic/status/1223077404660371456?s=20.
	 * 
	 * For later use, it seems that with the way debugging works, there is a significant overhead when it comes to
	 * runtime debug and the exception scope handling.
	 * 
	 * This would make using try-catch exceptions optional and there would be an alternative implementation...
	 */
	constexpr bool UseCpp_Exceptions = true;
}
