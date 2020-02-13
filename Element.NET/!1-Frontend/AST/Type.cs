using Lexico;

namespace Element.AST
{
    public class Type
    {
        [Literal(":"), WhitespaceSurrounded] private Unnamed _;
        [field: Term] public Identifier Identifier { get; }
    }
}