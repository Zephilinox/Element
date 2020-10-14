namespace Element
{
	using System;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Linq;

	/// <summary>
	/// Switch instruction, picks a result from many arguments based on passed in selector function
	/// </summary>
	public class Switch : Instruction
	{
		public static Result<Instruction> CreateAndOptimize(Instruction selector, IEnumerable<Instruction> operands, Context context)
		{
			var options = operands as Instruction[] ?? operands.ToArray();
			if (options.Length == 1 || options.All(o => o.Equals(options[0]))) return options[0];
			if (selector is Constant index)
			{
				if (float.IsNaN(index.Value))
				{
					// TODO: Return error type, not an error message
					return context.Trace(EleMessageCode.ArgumentOutOfRange, "Switch selector cannot be NaN");
				}
				
				var asInt = (int) index.Value;
				var inRange = asInt >= 0 && asInt <= options.Length;
				return inRange
					       ? new Result<Instruction>(options[asInt])
					       // TODO: Return error type, not an error message
					       : context.Trace(EleMessageCode.ArgumentOutOfRange, $"'{asInt}' not in range of {options.Length} cases specified");
			}
			return new Switch(selector, options);
		}
		
		private Switch(Instruction selector, IEnumerable<Instruction> operands)
		{
			Selector = selector ?? throw new ArgumentNullException(nameof(selector));
			Operands = new ReadOnlyCollection<Instruction>(operands.ToArray());
			if (Operands.Any(o => o == null))
			{
				throw new ArgumentException("An operand was null");
			}
		}

		public Instruction Selector { get; }
		public ReadOnlyCollection<Instruction> Operands { get; }

		public override IEnumerable<Instruction> Dependent => Operands.Concat(new[] {Selector});
		public override string SummaryString => $"Switch(${Selector})[{ListJoinToString(Operands)}]";

		private int? _hashCode;

		public override int GetHashCode()
		{
			if (!_hashCode.HasValue)
			{
				var c = Selector.GetHashCode();
				for (var i = 0; i < Operands.Count; i++)
				{
					c ^= Operands[i].GetHashCode();
				}

				_hashCode = c;
			}

			return _hashCode.Value;
		}

		public override bool Equals(Instruction other)
		{
			if (this == other) return true;
			if (!(other is Switch bOther) || bOther.Operands.Count != Operands.Count || !bOther.Selector.Equals(Selector))
				return false;
			for (var i = 0; i < Operands.Count; i++)
			{
				if (!Operands[i].Equals(bOther.Operands[i]))
				{
					return false;
				}
			}

			return true;
		}
	}
}