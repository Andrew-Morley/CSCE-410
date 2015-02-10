(TeX-add-style-hook "report"
 (lambda ()
    (LaTeX-add-environments
     "theorem"
     "lemma"
     "proposition"
     "corollary"
     "remark"
     "definition"
     "solution"
     "exercise"
     "problem")
    (TeX-add-symbols
     '("vect" 1)
     '("brac" 1)
     '("abs" 1)
     '("ceil" 1)
     '("floor" 1)
     '("set" 1)
     '("horrule" 1)
     "name"
     "course"
     "homeworkNumber"
     "duedate"
     "N"
     "Z"
     "Q"
     "R"
     "C"
     "Zp"
     "Qp"
     "Rp")
    (TeX-run-style-hooks
     "url"
     "pdfpages"
     "inputenc"
     "utf8"
     "color"
     "dvipsnames"
     "usenames"
     "algorithm2e"
     "listings"
     "fancyhdr"
     "amsthm"
     "amsmath"
     "enumerate"
     "setspace"
     "amssymb"
     "graphicx"
     "geometry"
     ""
     "latex2e"
     "art10"
     "article"
     "10pt")))

