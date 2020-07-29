using Element.AST;
using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json.Linq;

namespace Element
{
	/// <summary>
	/// Provides function arguments from a JObject loaded from file or a json string.
	/// </summary>
	public class JsonArgumentProvider
	{
		public JObject JsonConfiguration { get; private set; }

		public Result ProvideArguments(IReadOnlyList<ResolvedPort> topLevelFunctionPorts, float[] arguments, ITrace trace)
		{
			var builder = new ResultBuilder(trace);
			var idx = 0;

			void ProvisionObject(IReadOnlyList<ResolvedPort> ports, JObject jObject)
			{
				foreach (var input in ports)
				{
					if (input.Identifier.HasValue)
					{
						if (jObject.TryGetValue(input.Identifier.Value.String, out var jToken))
						{
							ProvisionSingleField(input.Identifier.Value, input.ResolvedConstraint, jToken);
						}
					}
					else
					{
						builder.Append(MessageCode.InvalidBoundaryFunction, "Port(s) with no identifier(s) which cannot be sourced. Boundary only supports named ports!");
					}
				}
			}
			
			void ProvisionSingleField(Identifier fieldIdentifier, IValue type, JToken value)
			{
				if (type.IsIntrinsic<NumStruct>())
				{
					switch (value?.Type ?? JTokenType.None)
					{
						case JTokenType.Float:
							arguments[idx++] = (float) (double) value!;
							break;
						case JTokenType.Integer:
							arguments[idx++] = (int) value!;
							break;
						default:
							builder.Append(MessageCode.InvalidBoundaryData, $"Expected float or integer token for element Num parameter '{fieldIdentifier}'");
							break;
					}
				}
				else if (type.IsIntrinsic<BoolStruct>())
				{
					switch (value?.Type ?? JTokenType.None)
					{
						case JTokenType.Boolean:
							arguments[idx++] = (bool) value! ? 1f : 0f;
							break;
						default:
							builder.Append(MessageCode.InvalidBoundaryData, $"Expected boolean token for element Bool parameter '{fieldIdentifier}'");
							break;
					}
				}
				else if (type is Struct declaredStruct)
				{
					switch (value?.Type ?? JTokenType.None)
					{
						case JTokenType.Object:
							ProvisionObject(declaredStruct.Fields, (JObject)value!);
							break;
						default:
							builder.Append(MessageCode.InvalidBoundaryData, $"Expected object token for element Struct parameter '{fieldIdentifier}'");
							break;
					}
				}
				else
				{
					builder.Append(MessageCode.InvalidBoundaryFunction, $"Element constraint '{type}' is not supported for JSON argument provisioning");
				}
			}

			ProvisionObject(topLevelFunctionPorts, JsonConfiguration);
			return builder.ToResult();
		}

		public Result ParseFromJsonFile(string filePath, ITrace trace)
		{
			if (!File.Exists(filePath)) return trace.Trace(MessageCode.FileAccessError, $"\"{filePath}\" JSON file not found.");
			string fileText;
			try
			{
				fileText = File.ReadAllText(filePath);
			}
			catch (Exception e)
			{
				return trace.Trace(MessageCode.FileAccessError, e.ToString());
			}
			return ParseFromJsonString(fileText, trace);
		}

		public Result ParseFromJsonString(string json, ITrace trace)
		{
			try
			{
				JsonConfiguration = JObject.Parse(json);
			}
			catch (Exception e)
			{
				return trace.Trace(MessageCode.ParseError, e.ToString());
			}

			return Result.Success;
		}
	}
}