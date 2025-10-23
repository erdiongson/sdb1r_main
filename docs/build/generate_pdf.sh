#!/bin/bash

# Generate PDF documentation from parent directory markdown files
cd "$(dirname "$0")/.." || exit

pandoc 1-overview.md 2-states.md 3-errors.md 4-controllers.md 5-dispense-cycle.md 6-skipping-mechanism.md 7-plc.md \
  -o "build/XentiQ 1R Technical Documentation.pdf" \
  -V geometry:a4paper \
  -V geometry:margin=0.5in \
  -V fontsize=12pt \
  -V mainfont="Helvetica" \
  -V monofont="Menlo" \
  --pdf-engine=xelatex

echo "PDF generated: build/XentiQ 1R Technical Documentation.pdf"
