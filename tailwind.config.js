/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ['./*.html'],
  theme: {
    screens: {
      sm:	'640px',
      md:	'786px',
      lg:	'1024px',
      xl:	'1280px',
    },
    extend: {},
  },
  plugins: [],
}




.bg-transparent {
  background-color: #000000;
  opacity: 0.5;
  padding-left: 19px;
  padding-right: 19px;
  padding-top: 32px;
  padding-bottom: 32px;
  border-radius: 15px;
  margin-bottom: 7.8rem;
}

.bg-transparent h1 {
 color: #71fff8 !important;
 margin-bottom: 1.9em;
}

.bg-transparent p {
 font-size: 20px;
 color: #fff !important;
}
