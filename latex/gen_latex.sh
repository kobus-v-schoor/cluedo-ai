#! /bin/bash

LATEX="code.tex"

cat > "$LATEX" <<\EOF
\documentclass{article}
\usepackage[a4paper, margin=0.50in]{geometry}
\usepackage[hidelinks]{hyperref}
\usepackage{minted}

\title{EPE321 - Code submission (AI)}
\author{Kobus van Schoor - u17005460}
\date{October 2019}

\begin{document}

\pagenumbering{gobble}
\maketitle
\newpage

\pagenumbering{roman}
\tableofcontents
\newpage

\pagenumbering{arabic}

EOF

for file in $(find .. -name '*.cpp' -or -name '*.h'); do
	file=${file:3}
	cat >> "$LATEX" <<-EOF
	\\section{${file}}
	\\inputminted[fontsize=\\small,breaklines]{cpp}{../${file}}
	\\newpage
	EOF
done

echo "\\end{document}" >> "$LATEX"

mkdir -p pdf
pdflatex -shell-escape -output-directory pdf "$LATEX"
