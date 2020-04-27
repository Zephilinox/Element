using System;
using System.Collections.Generic;
using Element.AST;
using LExpression = System.Linq.Expressions.Expression;

namespace Element.CLR
{
    public delegate System.Linq.Expressions.Expression? ConvertFunction(IValue value, Type outputType, Context context);
    
    public interface IBoundaryConverter
    {
        IValue LinqToElement(System.Linq.Expressions.Expression parameter, IBoundaryConverter root, Context compilationContext);
        System.Linq.Expressions.Expression? ElementToLinq(IValue value, Type outputType, ConvertFunction convertFunction, Context compilationContext);
    }

    public class NumberConverter : IBoundaryConverter
    {
        private NumberConverter(){}
        public static NumberConverter Instance { get; } = new NumberConverter();
        public IValue LinqToElement(System.Linq.Expressions.Expression parameter, IBoundaryConverter root, Context compilationContext) =>
            new NumberExpression(parameter.Type switch
            {
                {} t when t == typeof(bool) => LExpression.Condition(parameter, LExpression.Constant(1f), LExpression.Constant(0f)),
                _ => LExpression.Convert(parameter, typeof(float))
            });

        public System.Linq.Expressions.Expression? ElementToLinq(IValue value, Type outputType, ConvertFunction convertFunction,
                                                                 Context compilationContext) =>
            (outputType, convertFunction(value, typeof(float), compilationContext)) switch
            {
                (_, {} result) when result.Type == outputType => result, // Correct type from convert, return directly
                ({} t, {} result) when t == typeof(bool) => LExpression.LessThanOrEqual(result, LExpression.Constant(0f)),
                (_, {} result) => LExpression.Convert(result, outputType),
                (_, _) => null
            };

        private class NumberExpression : Expression, ICLRExpression
        {
            public NumberExpression(LExpression parameter) => Parameter = parameter;
            public LExpression Parameter { get; }
            public override IEnumerable<Expression> Dependent => Array.Empty<Expression>();
            public LExpression Compile(Func<Expression, LExpression> compileOther) => Parameter;
            protected override string ToStringInternal() => Parameter.ToString();
            public override bool Equals(Expression other) => other == this;
            public override int GetHashCode() => Parameter.GetHashCode();
        }
    }
    
    
    
    public class BoundaryConverter : Dictionary<Type, IBoundaryConverter>, IBoundaryConverter
    {
        public BoundaryConverter() : this(CreateDefault()) { }
        public BoundaryConverter(IDictionary<Type, IBoundaryConverter> mappings) : base(mappings) { }
        public static Dictionary<Type, IBoundaryConverter> CreateDefault()
        {
            /*var vector2 = new Input1D(new Map {{"x", "X"}, {"y", "Y"}});
            var rect = new Input1D(new Map {{"x", "X"}, {"y", "Y"}, {"width", "Width"}, {"height", "Height"}});*/
            return new Dictionary<Type, IBoundaryConverter>
            {
                {typeof(float), NumberConverter.Instance},
                {typeof(int), NumberConverter.Instance},
                {typeof(double), NumberConverter.Instance},
                {typeof(long), NumberConverter.Instance},
                {typeof(bool), NumberConverter.Instance},
                /*{typeof(Vector2), vector2},
                {typeof(Point), vector2},
                {typeof(PointF), vector2},
                {typeof(Vector3), new Input1D(new Map {{"x", "X"}, {"y", "Y"}, {"z", "Z"}})},
                {typeof(Vector4), new Input1D(new Map {{"x", "X"}, {"y", "Y"}, {"z", "Z"}, {"w", "W"}})},
                {typeof(Rectangle), rect},
                {typeof(RectangleF), rect},
                {
                    typeof(Matrix4x4), new Input2D(new Dictionary<string, object>
                    {
                        {"x", new Map {{"x", "M11"}, {"y", "M12"}, {"z", "M13"}, {"w", "M14"}}},
                        {"y", new Map {{"x", "M21"}, {"y", "M22"}, {"z", "M23"}, {"w", "M24"}}},
                        {"z", new Map {{"x", "M31"}, {"y", "M32"}, {"z", "M33"}, {"w", "M34"}}},
                        {"w", new Map {{"x", "M41"}, {"y", "M42"}, {"z", "M43"}, {"w", "M44"}}}
                    })
                },
                {
                    typeof(Matrix3x2), new Input2D(new Dictionary<string, object>
                    {
                        {"x", new Map {{"x", "M11"}, {"y", "M12"}}},
                        {"y", new Map {{"x", "M21"}, {"y", "M22"}}},
                        {"z", new Map {{"x", "M31"}, {"y", "M32"}}}
                    })
                },
                {typeof(float[]), new Array1D()}*/
            };
        }

        public IValue LinqToElement(System.Linq.Expressions.Expression parameter, IBoundaryConverter root, Context compilationContext)
        {
            if (TryGetValue(parameter.Type, out var retval))
            {
                return retval.LinqToElement(parameter, root, compilationContext);
            }
            
            // TODO: Generate converters

            return compilationContext.LogError(12, $"No {nameof(IBoundaryConverter)} for CLR type '{parameter.Type}'");
        }

        public System.Linq.Expressions.Expression? ElementToLinq(IValue value, Type outputType, ConvertFunction convertFunction,
                                                                 Context compilationContext)
        {
            // TODO: Detect circular
            if (TryGetValue(outputType, out var output))
            {
                return output.ElementToLinq(value, outputType, convertFunction, compilationContext);
            }

            compilationContext.LogError(12, $"No {nameof(IBoundaryConverter)} for CLR type '{outputType}'");
            return null;
        }
    }
}