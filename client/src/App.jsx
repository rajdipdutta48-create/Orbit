import { useState } from "react";
import "./App.css";

function highlightOrbitCode(code) {
  return code
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(
      /(".*?")/g,
      '<span class="syntax-string">$1</span>'
    )
    .replace(
      /('.*?')/g,
      '<span class="syntax-char">$1</span>'
    )
    .replace(
      /\b(dock|transmit|receive|when|else|orbiting|nebula)\b/g,
      '<span class="syntax-keyword">$1</span>'
    )
    .replace(
      /\b(true|false)\b/g,
      '<span class="syntax-boolean">$1</span>'
    )
    .replace(
      /\b\d+(\.\d+)?\b/g,
      '<span class="syntax-number">$&</span>'
    )
    .replace(
      /(\/\/.*$|comet:.*?burn)/gm,
      '<span class="syntax-comment">$1</span>'
    );
}

/*
 * Orbit documentation.
 *
 * Keeping the documentation as data makes it much easier
 * to expand the language reference later without creating
 * a huge block of difficult-to-maintain JSX.
 */
const documentationSections = [
  {
    title: "1. What is Orbit?",
    content: [
      {
        type: "text",
        value:
          "Orbit is a small, space-themed programming language. " +
          "It is designed to make programming concepts easy to read " +
          "and understand.",
      },
      {
        type: "text",
        value:
          "If you are completely new to programming, start from the " +
          "first section and move downward. Each section introduces " +
          "one idea and then shows a small example.",
      },
      {
        type: "note",
        value:
          "Orbit programs are made from statements. Most statements " +
          "end with a semicolon (;).",
      },
    ],
  },

  {
    title: "2. Your First Orbit Program",
    content: [
      {
        type: "code",
        value: `dock message = "Hello Orbit";
transmit(message);`,
      },
      {
        type: "text",
        value:
          "The first line creates a variable called message and stores " +
          "the text \"Hello Orbit\" inside it.",
      },
      {
        type: "text",
        value:
          "The second line sends the value stored in message to the " +
          "Mission Console.",
      },
      {
        type: "result",
        value: `Hello Orbit`,
      },
      {
        type: "tip",
        value:
          "When learning Orbit, start with very small programs. " +
          "Run them, understand the result, and then add another idea.",
      },
    ],
  },

  {
    title: "3. Variables",
    content: [
      {
        type: "text",
        value:
          "A variable is a named place where a program keeps a value. " +
          "You can think of it like a labeled box.",
      },
      {
        type: "code",
        value: `dock age = 20;
dock score = 95;
dock price = 99.50;`,
      },
      {
        type: "text",
        value:
          "Here, age, score, and price are variable names. " +
          "The values stored inside them are 20, 95, and 99.50.",
      },
      {
        type: "breakdown",
        items: [
          "dock → tells Orbit that a new variable is being created.",
          "age → the name of the variable.",
          "= → puts the value on the right into the variable.",
          "20 → the value being stored.",
          "; → marks the end of the statement.",
        ],
      },
    ],
  },

  {
    title: "4. Variable Initialization",
    content: [
      {
        type: "text",
        value:
          "When you create a variable with dock, Orbit expects you to " +
          "give it an initial value.",
      },
      {
        type: "code",
        label: "Correct",
        value: `dock age = 0;
dock name = "";
dock letter = 'A';`,
      },
      {
        type: "code",
        label: "Incorrect",
        value: `dock name;`,
      },
      {
        type: "text",
        value:
          "The incorrect example creates a variable without telling " +
          "Orbit what value it should initially contain.",
      },
      {
        type: "tip",
        value:
          "If you want an empty string, use \"\". If you want a number " +
          "that starts at zero, use 0.",
      },
    ],
  },

  {
    title: "5. Numbers",
    content: [
      {
        type: "text",
        value:
          "Numbers can be used for counting, calculations, comparisons, " +
          "scores, measurements, and many other tasks.",
      },
      {
        type: "code",
        value: `dock age = 20;
dock temperature = 25.5;
dock score = 100;`,
      },
      {
        type: "text",
        value:
          "Orbit can work with whole numbers and decimal numbers.",
      },
    ],
  },

  {
    title: "6. Strings",
    content: [
      {
        type: "text",
        value:
          "A string is text made from zero or more characters. " +
          "Strings are written between double quotes.",
      },
      {
        type: "code",
        value: `dock name = "Orbit";
dock message = "Hello World";
dock empty = "";`,
      },
      {
        type: "text",
        value:
          "Everything between the opening and closing double quotes " +
          "belongs to the string.",
      },
      {
        type: "note",
        value:
          "Use double quotes for strings: \"Hello\"",
      },
    ],
  },

  {
    title: "7. Characters",
    content: [
      {
        type: "text",
        value:
          "A character represents one character. Orbit writes a " +
          "character between single quotes.",
      },
      {
        type: "code",
        value: `dock grade = 'A';
dock symbol = '*';
dock letter = 'R';`,
      },
      {
        type: "text",
        value:
          "A single character uses single quotes. Multiple characters " +
          "should normally be written as a string.",
      },
      {
        type: "code",
        label: "Character",
        value: `'A'`,
      },
      {
        type: "code",
        label: "String",
        value: `"ABC"`,
      },
      {
        type: "tip",
        value:
          "Think: 'A' is one character, while \"ABC\" is text containing " +
          "three characters.",
      },
    ],
  },

  {
    title: "8. Boolean Values",
    content: [
      {
        type: "text",
        value:
          "A boolean is a value that can only be true or false.",
      },
      {
        type: "code",
        value: `dock running = true;
dock finished = false;`,
      },
      {
        type: "text",
        value:
          "Booleans are especially useful when making decisions with " +
          "when and when combining conditions.",
      },
    ],
  },

  {
    title: "9. Comments",
    content: [
      {
        type: "text",
        value:
          "A comment is information written for humans. Orbit does not " +
          "execute the contents of a comment.",
      },
      {
        type: "code",
        label: "Orbit comment",
        value: `comet: This is a comment burn`,
      },
      {
        type: "text",
        value:
          "Orbit's comment syntax starts with comet: and ends with burn. " +
          "Everything between them is ignored by the language.",
      },
      {
        type: "code",
        value: `comet:
This program stores an age.
The comment helps explain the code.
burn

dock age = 20;
transmit(age);`,
      },
      {
        type: "tip",
        value:
          "Use comments to explain why something exists or what a " +
          "larger section of your program is doing.",
      },
    ],
  },

  {
    title: "10. Output with transmit",
    content: [
      {
        type: "text",
        value:
          "Use transmit when you want Orbit to display a value.",
      },
      {
        type: "code",
        value: `transmit(25);
transmit("Hello Orbit");
transmit(true);`,
      },
      {
        type: "text",
        value:
          "You can also transmit a variable.",
      },
      {
        type: "code",
        value: `dock age = 20;
transmit(age);`,
      },
      {
        type: "result",
        value: `20`,
      },
      {
        type: "text",
        value:
          "The expression inside the parentheses is evaluated first. " +
          "The resulting value is then displayed.",
      },
    ],
  },

  {
    title: "11. Input with receive",
    content: [
      {
        type: "text",
        value:
          "Use receive when your program needs the user to provide a value.",
      },
      {
        type: "code",
        value: `dock age = 0;

receive(age);

transmit(age);`,
      },
      {
        type: "text",
        value:
          "Enter the input in the Program Input box of the IDE. " +
          "Orbit reads the values in the same order as the receive " +
          "statements appear.",
      },
      {
        type: "code",
        label: "Program with two inputs",
        value: `dock age = 0;
dock score = 0;

receive(age);
receive(score);

transmit(age);
transmit(score);`,
      },
      {
        type: "code",
        label: "Program Input",
        value: `21
95`,
      },
      {
        type: "text",
        value:
          "The first value is read by receive(age). The second value " +
          "is read by receive(score).",
      },
    ],
  },

  {
    title: "12. Assignment",
    content: [
      {
        type: "text",
        value:
          "The = operator assigns a value to a variable. In simple " +
          "terms, it changes what is stored in the variable.",
      },
      {
        type: "code",
        value: `dock age = 20;

age = 25;

transmit(age);`,
      },
      {
        type: "result",
        value: `25`,
      },
      {
        type: "breakdown",
        items: [
          "First, age contains 20.",
          "age = 25 changes the stored value to 25.",
          "transmit(age) prints the new value.",
        ],
      },
    ],
  },

  {
    title: "13. Arithmetic Operators",
    content: [
      {
        type: "text",
        value:
          "Arithmetic operators perform calculations on numbers.",
      },
      {
        type: "operator",
        items: [
          "+  → addition",
          "-  → subtraction",
          "*  → multiplication",
          "/  → division",
        ],
      },
      {
        type: "code",
        value: `transmit(10 + 5);
transmit(10 - 5);
transmit(10 * 5);
transmit(10 / 5);`,
      },
      {
        type: "result",
        value: `15
5
50
2`,
      },
      {
        type: "text",
        value:
          "The operator tells Orbit what calculation should be performed " +
          "between the value on its left and the value on its right.",
      },
    ],
  },

  {
    title: "14. Unary Minus",
    content: [
      {
        type: "text",
        value:
          "A minus sign can also be used before one value to make it negative.",
      },
      {
        type: "code",
        value: `dock temperature = -10;
transmit(-25);`,
      },
      {
        type: "text",
        value:
          "Here the minus sign is not subtracting two numbers. It is " +
          "changing one number into its negative value.",
      },
    ],
  },

  {
    title: "15. Comparison Operators",
    content: [
      {
        type: "text",
        value:
          "Comparison operators ask questions about two values. " +
          "The answer is a boolean: true or false.",
      },
      {
        type: "operator",
        items: [
          "<   → is the left value smaller?",
          "<=  → is the left value smaller or equal?",
          ">   → is the left value greater?",
          ">=  → is the left value greater or equal?",
        ],
      },
      {
        type: "code",
        value: `transmit(10 < 20);
transmit(10 > 20);
transmit(10 >= 10);`,
      },
      {
        type: "result",
        value: `true
false
true`,
      },
    ],
  },

  {
    title: "16. Equality Operators",
    content: [
      {
        type: "text",
        value:
          "Equality operators compare values to determine whether they " +
          "are equal or different.",
      },
      {
        type: "operator",
        items: [
          "==  → equal to",
          "!=  → not equal to",
        ],
      },
      {
        type: "code",
        value: `transmit(10 == 10);
transmit(10 == 20);
transmit(10 != 20);`,
      },
      {
        type: "result",
        value: `true
false
true`,
      },
      {
        type: "warning",
        value:
          "Do not confuse = with ==. A single = assigns a value. " +
          "Two == signs compare values.",
      },
    ],
  },

  {
    title: "17. Logical NOT",
    content: [
      {
        type: "text",
        value:
          "The ! operator reverses a boolean value.",
      },
      {
        type: "code",
        value: `transmit(!true);
transmit(!false);`,
      },
      {
        type: "result",
        value: `false
true`,
      },
      {
        type: "text",
        value:
          "You can read ! as \"not\". Therefore !true means \"not true\".",
      },
    ],
  },

  {
    title: "18. Logical AND",
    content: [
      {
        type: "text",
        value:
          "The && operator means that both conditions must be true.",
      },
      {
        type: "code",
        value: `transmit(true && true);
transmit(true && false);
transmit(false && false);`,
      },
      {
        type: "result",
        value: `true
false
false`,
      },
      {
        type: "tip",
        value:
          "Think of AND as: \"Are both of these things true?\"",
      },
    ],
  },

  {
    title: "19. Logical OR",
    content: [
      {
        type: "text",
        value:
          "The || operator means that at least one of the conditions " +
          "must be true.",
      },
      {
        type: "code",
        value: `transmit(true || false);
transmit(false || true);
transmit(false || false);`,
      },
      {
        type: "result",
        value: `true
true
false`,
      },
      {
        type: "tip",
        value:
          "Think of OR as: \"Is at least one of these things true?\"",
      },
    ],
  },

  {
    title: "20. Parentheses",
    content: [
      {
        type: "text",
        value:
          "Parentheses can be used to group part of an expression. " +
          "The grouped expression is calculated first.",
      },
      {
        type: "code",
        value: `transmit((10 + 20) * 2);`,
      },
      {
        type: "text",
        value:
          "Orbit first calculates 10 + 20, giving 30. It then multiplies " +
          "30 by 2.",
      },
      {
        type: "result",
        value: `60`,
      },
      {
        type: "tip",
        value:
          "When a calculation becomes difficult to read, use parentheses " +
          "to make the intended order obvious.",
      },
    ],
  },

  {
    title: "21. Conditions with when",
    content: [
      {
        type: "text",
        value:
          "Programs often need to make decisions. Orbit uses when to " +
          "run a block of code only when a condition is true.",
      },
      {
        type: "code",
        value: `when (age >= 18) {
    transmit("Adult");
}`,
      },
      {
        type: "text",
        value:
          "You can read this as: \"When age is at least 18, print Adult.\"",
      },
      {
        type: "breakdown",
        items: [
          "when → starts the decision.",
          "(age >= 18) → the condition being checked.",
          "{ ... } → the code that runs when the condition is true.",
        ],
      },
    ],
  },

  {
    title: "22. else",
    content: [
      {
        type: "text",
        value:
          "else gives your program another path when the when condition " +
          "is false.",
      },
      {
        type: "code",
        value: `when (age >= 18) {
    transmit("Adult");
} else {
    transmit("Not an adult");
}`,
      },
      {
        type: "text",
        value:
          "Exactly one of the two blocks runs: the when block if the " +
          "condition is true, otherwise the else block.",
      },
    ],
  },

  {
    title: "23. orbiting Loops",
    content: [
      {
        type: "text",
        value:
          "A loop repeats a block of code. Orbit uses orbiting for this.",
      },
      {
        type: "code",
        value: `dock count = 1;

orbiting (count <= 5) {
    transmit(count);
    count = count + 1;
}`,
      },
      {
        type: "result",
        value: `1
2
3
4
5`,
      },
      {
        type: "breakdown",
        items: [
          "Orbit checks count <= 5.",
          "If it is true, the code inside the braces runs.",
          "count is increased by 1.",
          "Orbit checks the condition again.",
          "The loop stops when the condition becomes false.",
        ],
      },
    ],
  },

  {
    title: "24. Nebula Arrays",
    content: [
      {
        type: "text",
        value:
          "A nebula stores multiple values together. You can think of " +
          "it as a row of numbered boxes.",
      },
      {
        type: "code",
        value: `nebula numbers = [10, 20, 30];`,
      },
      {
        type: "text",
        value:
          "Orbit uses zero-based indexing. That means the first element " +
          "has index 0.",
      },
      {
        type: "operator",
        items: [
          "numbers[0] → 10",
          "numbers[1] → 20",
          "numbers[2] → 30",
        ],
      },
      {
        type: "tip",
        value:
          "The number inside [ ] is the position of the value you want.",
      },
    ],
  },

  {
    title: "25. Reading an Array",
    content: [
      {
        type: "code",
        value: `nebula numbers = [10, 20, 30];

transmit(numbers[0]);
transmit(numbers[2]);`,
      },
      {
        type: "result",
        value: `10
30`,
      },
      {
        type: "text",
        value:
          "numbers[0] means \"give me the value stored at index 0\".",
      },
    ],
  },

  {
    title: "26. Changing an Array",
    content: [
      {
        type: "code",
        value: `nebula numbers = [10, 20, 30];

numbers[1] = 99;

transmit(numbers[1]);`,
      },
      {
        type: "result",
        value: `99`,
      },
      {
        type: "text",
        value:
          "The assignment changes the value at index 1. The array is now:",
      },
      {
        type: "code",
        value: `[10, 99, 30]`,
      },
    ],
  },

  {
    title: "27. Array Input",
    content: [
      {
        type: "text",
        value:
          "receive can also store input directly into an array element.",
      },
      {
        type: "code",
        value: `nebula numbers = [10, 20, 30];

receive(numbers[1]);

transmit(numbers[1]);`,
      },
      {
        type: "text",
        value:
          "If the user enters 50, the value at index 1 becomes 50.",
      },
      {
        type: "result",
        value: `[10, 50, 30]`,
      },
    ],
  },

  {
    title: "28. Strings and Characters in Arrays",
    content: [
      {
        type: "text",
        value:
          "Nebula arrays can also contain strings and characters.",
      },
      {
        type: "code",
        value: `nebula planets = ["Earth", "Mars"];
nebula letters = ['A', 'B'];

transmit(planets[0]);
transmit(letters[1]);`,
      },
      {
        type: "result",
        value: `Earth
B`,
      },
    ],
  },

  {
    title: "29. Operator Summary",
    content: [
      {
        type: "operator",
        items: [
          "Arithmetic",
          "+   add",
          "-   subtract",
          "*   multiply",
          "/   divide",
          "",
          "Assignment",
          "=   store a value",
          "",
          "Comparison",
          "<   less than",
          "<=  less than or equal to",
          ">   greater than",
          ">=  greater than or equal to",
          "",
          "Equality",
          "==  equal to",
          "!=  not equal to",
          "",
          "Logical",
          "!   NOT",
          "&&  AND",
          "||  OR",
        ],
      },
      {
        type: "warning",
        value:
          "The meaning of an operator depends on what values you are " +
          "using it with. Orbit reports a runtime error when an operation " +
          "is not valid.",
      },
    ],
  },

  {
    title: "30. Common Beginner Mistakes",
    content: [
      {
        type: "mistake",
        title: "Forgetting the initial value",
        wrong: `dock name;`,
        correct: `dock name = "";`,
      },
      {
        type: "mistake",
        title: "Using double quotes for a character",
        wrong: `dock letter = "A";`,
        correct: `dock letter = 'A';`,
      },
      {
        type: "mistake",
        title: "Forgetting the semicolon",
        wrong: `dock age = 20`,
        correct: `dock age = 20;`,
      },
      {
        type: "mistake",
        title: "Confusing = and ==",
        wrong: `age == 20;`,
        correct: `age = 20;`,
      },
      {
        type: "mistake",
        title: "Using an invalid array index",
        wrong: `nebula numbers = [10, 20];
transmit(numbers[5]);`,
        correct:
          "Only use an index that exists in the array.",
      },
    ],
  },

  {
    title: "31. Understanding Errors",
    content: [
      {
        type: "text",
        value:
          "An error means Orbit could not understand your program or " +
          "could not execute an operation.",
      },
      {
        type: "text",
        value:
          "When an error contains a line number, go directly to that " +
          "line in the editor and inspect it.",
      },
      {
        type: "code",
        label: "Example",
        value: `Line 4:
dock age = ;`,
      },
      {
        type: "text",
        value:
          "The example tells you that line 4 contains the problem.",
      },
      {
        type: "text",
        value:
          "Also inspect the line immediately before the reported line. " +
          "A missing semicolon, parenthesis, or closing brace can sometimes " +
          "make the parser notice the problem on a later line.",
      },
      {
        type: "tip",
        value:
          "Read an error from left to right: first understand what kind " +
          "of error occurred, then look at the line number, then inspect " +
          "the code around that location.",
      },
    ],
  },

  {
    title: "32. Complete Example",
    content: [
      {
        type: "text",
        value:
          "The following program combines comments, variables, comparison, " +
          "conditions, and output.",
      },
      {
        type: "code",
        value: `comet: Check whether the age is enough burn

dock age = 20;

when (age >= 18) {
    transmit("Adult");
} else {
    transmit("Not an adult");
}`,
      },
      {
        type: "text",
        value:
          "Once each individual concept is familiar, combine them to build " +
          "larger programs.",
      },
    ],
  },
];

function Documentation({ onClose }) {
  return (
    <aside className="docs-panel">

      <div className="panel-header">

        <span>
          Orbit Documentation
        </span>

        <button
          className="close-button"
          onClick={onClose}
        >
          ×
        </button>

      </div>

      <div className="docs-content">

        {documentationSections.map((section, sectionIndex) => (
          <section
            className="docs-section"
            key={sectionIndex}
          >

            <h3>
              {section.title}
            </h3>

            {section.content.map((item, itemIndex) => {

              if (item.type === "text") {
                return (
                  <p key={itemIndex}>
                    {item.value}
                  </p>
                );
              }

              if (item.type === "code") {
                return (
                  <div
                    className="docs-code-block"
                    key={itemIndex}
                  >
                    {item.label && (
                      <div className="docs-code-label">
                        {item.label}
                      </div>
                    )}

                    <pre>
                      {item.value}
                    </pre>
                  </div>
                );
              }

              if (item.type === "result") {
                return (
                  <div
                    className="docs-result"
                    key={itemIndex}
                  >
                    <strong>Result</strong>
                    <pre>{item.value}</pre>
                  </div>
                );
              }

              if (item.type === "note") {
                return (
                  <div
                    className="docs-note"
                    key={itemIndex}
                  >
                    <strong>Note</strong>
                    <p>{item.value}</p>
                  </div>
                );
              }

              if (item.type === "tip") {
                return (
                  <div
                    className="docs-tip"
                    key={itemIndex}
                  >
                    <strong>Tip</strong>
                    <p>{item.value}</p>
                  </div>
                );
              }

              if (item.type === "warning") {
                return (
                  <div
                    className="docs-warning"
                    key={itemIndex}
                  >
                    <strong>Important</strong>
                    <p>{item.value}</p>
                  </div>
                );
              }

              if (item.type === "breakdown") {
                return (
                  <div
                    className="docs-breakdown"
                    key={itemIndex}
                  >
                    <strong>How to read it</strong>

                    <ul>
                      {item.items.map((point, index) => (
                        <li key={index}>
                          {point}
                        </li>
                      ))}
                    </ul>
                  </div>
                );
              }

              if (item.type === "operator") {
                return (
                  <div
                    className="docs-operators"
                    key={itemIndex}
                  >
                    {item.items.map((operator, index) => (
                      <div key={index}>
                        {operator}
                      </div>
                    ))}
                  </div>
                );
              }

              if (item.type === "mistake") {
                return (
                  <div
                    className="docs-mistake"
                    key={itemIndex}
                  >
                    <h4>
                      {item.title}
                    </h4>

                    <div>
                      <strong>Incorrect</strong>
                      <pre>{item.wrong}</pre>
                    </div>

                    <div>
                      <strong>Correct</strong>
                      <pre>{item.correct}</pre>
                    </div>
                  </div>
                );
              }

              return null;
            })}

          </section>
        ))}

      </div>

    </aside>
  );
}

function App() {
  const [code, setCode] = useState(`dock message = "Hello Orbit";
transmit(message);`);

  const [input, setInput] = useState("");

  const [output, setOutput] = useState(
    "Orbit engine ready. Write your program and launch it."
  );

  const [showDocs, setShowDocs] = useState(false);

  const lines = code.split("\n");

  // Send Orbit code and browser input to the backend.
  const runCode = async () => {
    setOutput("✦ Launching Orbit program...\n");

    try {
      const response = await fetch(
        "http://localhost:5000/api/run",
        {
          method: "POST",

          headers: {
            "Content-Type": "application/json",
          },

          body: JSON.stringify({
            code,
            input,
          }),
        }
      );

      const result = await response.json();

      if (result.success) {
        setOutput(
          "✓ Orbit program executed successfully.\n\n" +
            result.output
        );
      } else {
        setOutput(
          "⚠ Orbit execution failed.\n\n" +
            (result.error || "Unknown error")
        );
      }

    } catch (error) {
      setOutput(
        "⚠ Unable to connect to Orbit server.\n\n" +
          error.message
      );
    }
  };

  // Add four spaces when Tab is pressed.
  const handleKeyDown = (event) => {
    if (event.key === "Tab") {
      event.preventDefault();

      const start = event.target.selectionStart;
      const end = event.target.selectionEnd;

      const newCode =
        code.substring(0, start) +
        "    " +
        code.substring(end);

      setCode(newCode);

      requestAnimationFrame(() => {
        event.target.selectionStart = start + 4;
        event.target.selectionEnd = start + 4;
      });
    }
  };

  return (
    <div className="orbit-app">

      {/* Top navigation */}
      <header className="topbar">

        <div className="brand">

          <div className="orbit-symbol">
            ◉
          </div>

          <div>
            <h1>ORBIT</h1>
            <span>
              Programming Beyond Earth
            </span>
          </div>

        </div>

        <div className="top-actions">

          <button
            className="nav-button"
            onClick={() => setShowDocs(!showDocs)}
          >
            Documentation
          </button>

          <button
            className="run-button"
            onClick={runCode}
          >
            ▶ Launch
          </button>

        </div>

      </header>

      {/* Main IDE */}
      <main className="ide">

        {/* Code editor */}
        <section className="editor-panel">

          <div className="panel-header">

            <div className="file-name">

              <span className="file-dot"></span>

              main.orbit

            </div>

            <span className="language-status">
              Orbit
            </span>

          </div>

          <div className="editor-container">

            {/* Line numbers */}
            <div className="line-numbers">

              {lines.map((_, index) => (
                <div key={index}>
                  {index + 1}
                </div>
              ))}

            </div>

            {/* Syntax highlighted code */}
            <pre
              className="highlight-layer"
              aria-hidden="true"
              dangerouslySetInnerHTML={{
                __html:
                  highlightOrbitCode(code) +
                  "\n",
              }}
            />

            {/* Actual editor */}
            <textarea
              className="code-editor"
              value={code}
              onChange={(event) =>
                setCode(event.target.value)
              }
              onKeyDown={handleKeyDown}
              spellCheck="false"
              autoCapitalize="off"
              autoCorrect="off"
            />

          </div>

        </section>

        {/* Documentation */}
        {showDocs && (
          <Documentation
            onClose={() => setShowDocs(false)}
          />
        )}

        {/* Output */}
        <section className="output-panel">

          <div className="panel-header">

            <span>
              Mission Console
            </span>

            <span className="status">
              ● READY
            </span>

          </div>

          <pre className="output">
            {output}
          </pre>

        </section>

        {/* Browser input */}
        <section className="input-panel">

          <div className="panel-header">

            <span>
              Program Input
            </span>

          </div>

          <textarea
            className="program-input"
            value={input}
            onChange={(event) =>
              setInput(event.target.value)
            }
            placeholder="Enter values for receive()..."
            spellCheck="false"
          />

        </section>

      </main>

      {/* Bottom status bar */}
      <footer className="statusbar">

        <span>
          ✦ ORBIT ENGINE
        </span>

        <span>
          Lexer · Parser · AST · Interpreter
        </span>

        <span>
          SYSTEM READY
        </span>

      </footer>

    </div>
  );
}

export default App;