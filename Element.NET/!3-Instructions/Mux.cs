namespace Element
{
	using System;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Linq;

	/// <summary>
	/// Multiplexing instruction, picks a result from many arguments based on passed in selector function
	/// </summary>
	public class Mux : Instruction
	{
		public static Instruction CreateAndOptimize(Instruction selector, IEnumerable<Instruction> operands)
		{
			var options = operands as Instruction[] ?? operands.ToArray();
			if (options.Length == 1 || options.All(o => o.Equals(options[0]))) return options[0];
			if (selector is Constant index)
			{
				var idx = index.Value >= options.Length
					          ? options.Length - 1
					          : index.Value < 0
						          ? 0
						          : index.Value;
				return options[(int)idx];
			}
			return new Mux(selector, options);
		}
		
		private Mux(Instruction selector, IEnumerable<Instruction> operands)
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
		public override string SummaryString => $"[{ListJoinToString(Operands)}]:{Selector}";

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
			if (!(other is Mux bOther) || bOther.Operands.Count != Operands.Count || !bOther.Selector.Equals(Selector))
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