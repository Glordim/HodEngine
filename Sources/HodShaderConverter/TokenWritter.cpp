#include "TokenWritter.h"

namespace hod
{
	/// @brief 
	/// @param stream 
	/// @param tokens 
	/// @return 
	bool TokenWritter::PrettyWrite(std::ostream& stream, const std::vector<Token>& tokens)
	{
		int indent = 0;
		bool newLine = false;

		int count = tokens.size();
		for (int index = 0; index < count; ++index)
		{
			const Token& token = tokens[index];

			if (tokens[index]._type == Token::Type::Space)
			{
				continue;
			}
			else if (tokens[index]._type == Token::Type::NewLine)
			{
				newLine = true;
				stream << "\n";
				continue;
			}

			if (newLine == true)
			{
				for (int i = 0; i < indent; ++i)
				{
					stream << "\t";
				}
			}
			newLine = false;

			if (index > 0)
			{
				const Token& prevToken = tokens[index - 1];
				bool prevNeedSpace = (
					prevToken._type == Token::Type::Identifier ||
					prevToken._type == Token::Type::IntegerValue ||
					prevToken._type == Token::Type::FloatingPointValue
				);

				bool currentNeedSpace = (
					token._type == Token::Type::Identifier ||
					token._type == Token::Type::IntegerValue ||
					token._type == Token::Type::FloatingPointValue
				);
				
				if (prevNeedSpace == true && currentNeedSpace == true)
				{
					stream << " ";
				}
			}

			switch (token._type)
			{
				case Token::Type::Identifier:
				{
					stream << std::get<std::string>(token._data);
				}
				break;

				case Token::Type::StringValue:
				{
					stream << "\"" << std::get<std::string>(token._data) << "\"";
				}
				break;

				case Token::Type::IntegerValue:
				{
					stream << std::get<int>(token._data);
				}
				break;

				case Token::Type::FloatingPointValue:
				{
					stream << std::get<float>(token._data);
				}
				break;

				case Token::Type::OpenCurlyBracket:
				{
					stream << "\n";
					stream << Token::_labels[token._type];
					stream << "\n";
					++indent;
					newLine = true;
				}
				break;

				case Token::Type::ClosingCurlyBracket:
				{
					--indent;
					stream << "\n";
					stream << Token::_labels[token._type];
					if (index + 1 < count && tokens[index + 1]._type == Token::Semicolon)
					{
						stream << ";";
						++index;
					}
					stream << "\n";
					newLine = true;
				}
				break;

				default:
				{
					stream << Token::_labels[token._type];
				}
				break;
			}
		}

		stream.flush();
		return true;
	}

	/// @brief 
	/// @param stream 
	/// @param tokens 
	/// @return 
	bool TokenWritter::CompactWrite(std::ostream& stream, const std::vector<Token>& tokens)
	{
		return false;
	}
}
